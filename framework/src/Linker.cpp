#include "JesusVM/Linker.h"
#include "JesusVM/Preload.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/platform/NativePlugin.h"

#include "JesusVM/util/StringPool.h"

#include "moduleweb/module_info.h"
#include "moduleweb/stream.h"

#include <algorithm>
#include <condition_variable>
#include <filesystem>
#include <mutex>
#include <unordered_map>
#include <cassert>

#undef BOOL
#undef VOID

namespace fs = std::filesystem;

namespace ModuleWeb {
    class InStream {
    public:
        InStream()
            : mInit(false) {
        }

        ~InStream() {
            if (mInit) close();
        }

        int open(const std::string& name) {
            if (mInit) close();

            int res = moduleweb_instream_open(&mStream, name.c_str());
            if (!res) mInit = true;

            return res;
        }

        void close() {
            moduleweb_instream_close(&mStream);
            mInit = false;
        }

        moduleweb_instream* get() {
            return &mStream;
        }

    private:
        moduleweb_instream mStream;
        bool mInit;
    };
}

namespace JesusVM::Linker {
    struct ModuleKey {
        ModuleKey(std::string_view name, Object* linker)
            : name(name)
            , linker(linker) { }

        std::string_view name;
        Object* linker;

        struct Hash {
            std::size_t operator()(const ModuleKey& key) const {
                return std::hash<std::string_view>()(key.name) + 31 * std::hash<Object*>()(key.linker);
            }
        };

        struct Equals {
            bool operator()(const ModuleKey& lhs, const ModuleKey& rhs) const {
                if (lhs.linker != rhs.linker) {
                    return false;
                }

                return lhs.name == rhs.name;
            }
        };
    };

    struct LinkerModule {
        enum class Status {
            LOADING,
            LOADED,
            NOT_FOUND,
        };

        LinkerModule(Status status, std::unique_ptr<Module> module, u32 threadsWaiting, Handle loadingThread, Object* linker)
            : status(status)
            , module(std::move(module))
            , threadsWaiting(threadsWaiting)
            , loadingThread(loadingThread)
            , linker(linker) { }

        Status status;
        std::unique_ptr<Module> module;

        u32 threadsWaiting;
        Handle loadingThread;

        ObjectRef linker;
    };

    std::mutex mutex;
    std::mutex pluginMutex;
    std::condition_variable condition;

    std::vector<std::string_view> modulePath;
    std::unordered_map<ModuleKey, std::unique_ptr<LinkerModule>, ModuleKey::Hash, ModuleKey::Equals> modules;

    std::vector<std::string_view> pluginPath;
    std::unordered_map<std::string_view, Platform::NativePlugin> plugins;

    static std::array<std::unique_ptr<Class>, static_cast<u64>(Type::TYPE_COUNT)> primitiveCache;

    void Init() {
    }

    void Destroy() {
        modulePath.clear();
        modules.clear();
    }

    static void ParsePaths(std::vector<std::string_view>& paths, std::string_view path) {
        while (!path.empty()) {
            auto pos = path.find(':');

            std::string_view currentPath = path.substr(0, pos);

            std::error_code err;
            fs::file_status stat = fs::status(currentPath, err);

            if (!err && fs::exists(stat)) {
                if (fs::is_directory(stat)) {
                    paths.push_back(StringPool::Intern(currentPath));
                } else {
                    std::cout << "warning: '" << currentPath << "' is not a directory\n";
                }
            } else {
                if (!fs::exists(stat)) {
                    std::cout << "warning: '" << currentPath << "' does not exist\n";
                } else {
                    std::cout << "warning: failed to get file status for '" << currentPath << "'\nerror message: " << err.message() << "\n";
                }
            }

            if (pos != std::string_view::npos) {
                path.remove_prefix(pos + 1);
            } else {
                break;
            }
        }
    }

    void AddPath(std::string_view path) {
        ParsePaths(modulePath, path);
    }

    void AddPluginPath(std::string_view path) {
        ParsePaths(pluginPath, path);
    }

    static Platform::NativePlugin* FindPlugin(std::string_view name) {
        auto it = plugins.find(name);
        if (it != plugins.end()) {
            return &it->second;
        }

        return nullptr;
    }

    static Platform::NativePlugin* LoadPluginFromFile(std::string_view fileName, std::string_view name) {
        if (!fs::exists(fileName)) {
            return nullptr;
        }

        fileName = StringPool::Intern(fileName);
        name = StringPool::Intern(name);

        auto [it, inserted] = plugins.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(name),
                std::forward_as_tuple(name, fileName)
        );

        return &it->second;
    }

    static Platform::NativePlugin* LoadPluginFromPath(std::string_view path, std::string_view name) {
        std::string fullName(path);
        fullName += "/";
        fullName += name;
        fullName += Platform::NativePlugin::extension;

        return LoadPluginFromFile(fullName, name);
    }

    void LoadPlugin(std::string_view name) {
        std::lock_guard<std::mutex> lock(pluginMutex);

        Platform::NativePlugin* plugin = FindPlugin(name);

        if (plugin == nullptr) {
            for (std::string_view& path: pluginPath) {
                plugin = LoadPluginFromPath(path, name);
                if (plugin != nullptr) break;
            }
        }

        if (plugin != nullptr) {
            if (!plugin->load()) {
                std::cout << "warning: failed to load plugin '" << name << "'\n";
            }
            return; // already loaded
        }
    }

    static std::string GetNativeFunctionName(Function* function) {
        std::string name = "JesusVM_";
        name += function->getModule()->getName();
        name += '_';
        name += function->getName();

        if (!function->getArgumentTypes().empty()) {
            name += "__";

            for (auto& arg: function->getArgumentTypes()) {
                name += arg.getClassName();
            }
        }

        std::replace(name.begin(), name.end(), '/', '_');

        return std::move(name);
    }

    void LinkNativeFunction(Function* function) {
        std::string name = GetNativeFunctionName(function);

        for (auto& [_, plugin] : plugins) {
            auto func = plugin.getFunction<u8*>(name); // type doesn't matter for us here
            if (func == nullptr) continue;

            function->mCodeAttribute.code = func;
            return;
        }

        if (Preload::finished) { // only error if preloading has failed since preload could override the function entry
            std::cout << "error: unable to link native function " << function->getName() << function->getDescriptor() << "\n";
            std::exit(1);
        }
    }

    static inline void RemoveModule(ModuleKey& key) {
        modules.erase(key);
    }

    static LinkerModule* FindModule(std::unique_lock<std::mutex>& lock, Object* linker, std::string_view name) {
        ModuleKey key(name, linker);
        auto it = modules.find(key);
        LinkerModule* module = nullptr;

        if (it != modules.end()) {
            module = it->second.get();

            if (module->status == LinkerModule::Status::LOADING &&
                module->loadingThread == Threading::CurrentThread::GetHandle()) {
                return nullptr;
            }

            module->threadsWaiting++;
            condition.wait(lock, [module] {
                return module->status != LinkerModule::Status::LOADING;
            });
            module->threadsWaiting--;

            if (module->status == LinkerModule::Status::NOT_FOUND && module->threadsWaiting == 0) {
                RemoveModule(key);
                module = nullptr;
            }
        }

        return module;
    }

    static Module* LoadModuleWithLinker(Object* linker, std::string_view name) {
        /*
         TODO:
         Object* rtName = AllocString(name.replace('/', '.'); // std/Primitives:String
         Object* reflectionModule = linker.callMethod(Runtime::std::Reflection::Linker::loadModule, rtName); // std/Reflection:Linker.loadModule(Rstd/Primitives:String;)Rstd/Reflection:Module;

         if (reflectionModule == nullptr) {
            rtName.removeReference(); // our reference from allocating it
            return nullptr;
         }

         return GetModuleFromReflection(reflectionModule);
        */

        return nullptr;
    }

    static Module* LoadModuleFromFile(std::string& fileName, std::string_view name) {
        ModuleWeb::InStream stream;

        if (stream.open(fileName)) {
            return nullptr;
        }

        auto moduleInfo = static_cast<moduleweb_module_info*>(std::malloc(sizeof(moduleweb_module_info))); // c struct. using new and make_unique is apparently bad
        if (moduleweb_module_info_init(moduleInfo, name.data(), name.length(), stream.get())) {
            return nullptr;
        }

        auto result = new Module(nullptr, moduleInfo);
        return result;
    }

    static Module* LoadModuleFromPath(std::string_view path, std::string_view name) {
        std::string fullName(path);
        fullName += "/";
        fullName += name;
        fullName += ".jmod";

        return LoadModuleFromFile(fullName, name);
    }

    static Module* LoadModuleWithBootstrap(std::string_view name) {
        Module* result = nullptr;

        for (std::string_view& path : modulePath) {
            result = LoadModuleFromPath(path, name);
            if (result != nullptr) break;
        }

        if (result != nullptr) {
            result->mLinker = nullptr;
        }

        return result;
    }

    Module* LoadModule(Object* linker, std::string_view name) {
        name = StringPool::Intern(name); // intern string so we don't get fucked by any weird people loading modules

        if (linker != nullptr) {
            return LoadModuleWithLinker(linker, name);
        }

        std::unique_lock<std::mutex> lock(mutex);

        LinkerModule* module = FindModule(lock, linker, name);
        if (module != nullptr) {
            if (module->status == LinkerModule::Status::LOADED) {
                return module->module.get();
            }

            return nullptr; // TODO: ?
        }

        auto [it, success] = modules.emplace(
                ModuleKey(name, linker),
                std::make_unique<LinkerModule>(
                        LinkerModule(LinkerModule::Status::LOADING, nullptr, 0, Threading::CurrentThread::GetHandle(), linker)));

        if (!success) {
            return nullptr;
        }

        module = it->second.get();

        lock.unlock();

        Module* vmModule = LoadModuleWithBootstrap(name);

        lock.lock();

        if (vmModule == nullptr) {
            if (module->threadsWaiting == 0) {
                RemoveModule(linker, name);
            } else {
                module->status = LinkerModule::Status::NOT_FOUND;
            }
        } else {
            module->module = std::unique_ptr<Module>(vmModule);
            module->status = LinkerModule::Status::LOADED;

            lock.unlock();

            Function* init = vmModule->getFunction("#LinkInit", "()V");
            if (init != nullptr) {
                init->invoke<void>();
            }

            lock.lock();
        }

        condition.notify_all();

        return vmModule;
    }

    static Module* LoadSystemModule() { // this intentionally doesn't use rt::vm::System
        static Module* cache = nullptr;

        if (cache == nullptr) {
            cache = LoadModule(nullptr, "vm/System");
        }

        return cache;
    }

    void RemoveModule(Object* linker, std::string_view name) {
        ModuleKey key(name, linker);
        RemoveModule(key);
    }

    static Class* FindClass(std::unique_lock<std::mutex>& lock, Module* module, std::string_view name) {
        Class* clas = module->findClass(name);

        if (clas == nullptr) {
            return nullptr;
        }

        if (clas->getState() == ClassState::LINKING
            && clas->mLoadingThread == Threading::CurrentThread::GetHandle()) {
            return nullptr;
        }

        clas->mWaitingThreads++;

        condition.wait(lock, [clas] {
            return clas->getState() != ClassState::LINKING;
        });

        clas->mWaitingThreads--;

        if (clas->getState() == ClassState::ERRORED && clas->mWaitingThreads != 0) {
            module->mClasses.erase(name);
            clas = nullptr;
        }

        return clas;
    }

    static Class* LoadClassWithLinker(Object* linker, Module* module, std::string_view name) {
        /*
         TODO:

         Object* rtName = AllocString(name.replace('/', '.'); // std/Primitives:String
         Object* reflectionModule = GetModuleObject(module);
         Object* reflectionClass = linker.callMethod(Runtime::std::Reflection::Linker::loadClass, reflectionModule, rtName); // std/Reflection:Linker.loadClass(Rstd/Reflection:Module;Rstd/Primitives:String;)Rstd/Reflection:Class;

         if (reflectionClass == nullptr) {
            rtName.removeReference(); // our reference from allocating it
            return nullptr;
         }

         return GetClassFromReflection(reflectionClass);
         */

        return nullptr;
    }

    static Class* LoadClassWithBootstrap(Module* module, std::string_view name) {
        Class* clas = module->findClass(name);

        if (clas->link()) {
            return nullptr;
        }

        return clas;
    }

    static std::string_view GetArrayBaseName(std::string_view name) {
        auto index = name.find('[');

        if (index == std::string_view::npos) {
            return {}; // closest possible to null :pray:
        }

        if (name[index + 1] == 'R') {
            return name.substr(index + 2, name.length() - index - 3); // skip R and ;
        }

        return name.substr(index + 1);
    }

    static Class* LoadArrayBaseClass(Module* module, std::string_view name) {
        while (name.size() > 1 && name[1] == '[') {
            name.remove_prefix(1);
        }

        std::string_view baseName = GetArrayBaseName(name);

        Class* result;
        Type type = StringToType(baseName);

        if (type != Type::REFERENCE) {
            result = primitiveCache[static_cast<u64>(type)].get();
        } else {
            result = LoadClass(module, baseName);
        }

        return result;
    }

    static Class* LoadArrayClass(Module* module, std::string_view name) {
        assert(name[0] == '[');

        auto arrayClass = new Class(module, nullptr);
        Class* baseClass;
        std::string_view baseClassName = GetArrayBaseName(name);

        if (baseClassName.empty()) {
            delete arrayClass;
            return nullptr;
        }

        Type type = StringToType(name.substr(1));

        if (type != Type::REFERENCE) {
            baseClass = primitiveCache[static_cast<u64>(type)].get();
        } else {
            baseClass = LoadClass(module, baseClassName);
        }

        if (baseClass == nullptr) {
            delete arrayClass;
            return nullptr;
        }

        if (arrayClass->linkArray(baseClass, name)) {
            // TODO: error mayhaps
            delete arrayClass;
            return nullptr;
        }

        return arrayClass;
    }

    Class* LoadClass(Module* module, std::string_view name) {
        name = StringPool::Intern(name);

        TypeInfo basicType(name);

        bool systemModule = false;

        if (module == nullptr) {
            module = LoadSystemModule();
            systemModule = true;
        }

        if (module->mLinker != nullptr) {
            if (basicType.isPrimitiveArrayType() && !systemModule) {
                module = LoadSystemModule();
                systemModule = true;
            }

            if (!basicType.isArray()) {
                return LoadClassWithLinker(module->mLinker, module, name);
            }

            Class* base = LoadArrayBaseClass(module, name);
            if (base == nullptr) {
                return nullptr;
            }
        }

        std::unique_lock<std::mutex> lock(mutex);

        Class* clas = FindClass(lock, module, name);
        if (clas != nullptr) {
            lock.unlock();

            link_check:
            if (clas->getState() == ClassState::LINKED) {
                return clas;
            } else if (clas->getState() == ClassState::INITIALIZED) {
                if (!basicType.isArray()) {
                    if (clas->link()) {
                        return nullptr;
                    }
                }

                goto link_check;
            }

            return nullptr;
        }

        lock.unlock();

        if (basicType.isArray()) {
            clas = LoadArrayClass(module, name);

            if (clas == nullptr) {
                return nullptr;
            }
        } else {
            return nullptr; // class doesn't exist and can't be made as an artificial class
        }

        lock.lock();

        if (clas->mState == ClassState::ERRORED) {
            if (clas->mWaitingThreads == 0) {
                module->mClasses.erase(name);
            }

            return nullptr;
        }

        module->mClasses.emplace(name, clas);

        condition.notify_all();

        return clas;
    }

    Class* LoadClass(std::string_view qualifiedName, Object* linker) {
        std::size_t arrayDepth = 0;
        while (arrayDepth < qualifiedName.size() && qualifiedName[arrayDepth] == '[') {
            ++arrayDepth;
        }

        std::string_view coreTypeName = qualifiedName.substr(arrayDepth);

        auto index = qualifiedName.find(':');
        if (index == std::string_view::npos) {
            return LoadClass(nullptr, qualifiedName);
        }

        std::string_view moduleName = coreTypeName.substr(0, index);
        std::string_view className = coreTypeName.substr(index + 1);

        Module* module = LoadModule(linker, moduleName);

        if (arrayDepth > 0) {
            std::string fullClassName(qualifiedName.substr(0, arrayDepth));
            fullClassName += className;

            return LoadClass(module, fullClassName);
        } else {
            return LoadClass(module, className);
        }
    }

    static Type TypeFromPrimitiveName(std::string_view name) {
        if (name == "void")     return Type::VOID;
        if (name == "bool")     return Type::BOOL;
        if (name == "byte")     return Type::BYTE;
        if (name == "short")    return Type::SHORT;
        if (name == "int")      return Type::INT;
        if (name == "long")     return Type::LONG;
        if (name == "char")     return Type::CHAR;
        if (name == "float")    return Type::FLOAT;
        if (name == "double")   return Type::DOUBLE;
        if (name == "handle")   return Type::HANDLE;

        return Type::TYPE_COUNT;
    }

    Class* LoadPrimitive(std::string_view name) {
        Type type = TypeFromPrimitiveName(name);
        if (type == Type::TYPE_COUNT) {
            std::cout << "error: invalid primitive class name: " << name << std::endl;
            std::exit(1);
        }

        if (primitiveCache[static_cast<u64>(type)] != nullptr) {
            return primitiveCache[static_cast<u64>(type)].get();
        }

        Module* system = LoadSystemModule();
        auto* clas = new Class(system, nullptr);

        clas->mRepresentedPrimitive = type;

        if (clas->linkPrimitive(name)) {
            return nullptr;
        }

        primitiveCache[static_cast<u64>(type)] = std::unique_ptr<Class>(clas);

        return clas;
    }
}
