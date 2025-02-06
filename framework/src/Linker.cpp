#include "JesusVM/Linker.h"

#include "JesusVM/util/StringPool.h"

#include <filesystem>
#include <mutex>
#include <unordered_map>

namespace fs = std::filesystem;

namespace JesusVM::Linker {
    struct ModuleKey {
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

        Status status;
        std::unique_ptr<Module> module;

        Object* linker;
    };

    JesusVM* globalVM;

    std::mutex mutex;
    std::condition_variable condition;

    std::vector<std::string_view> modulePath;
    std::unordered_map<ModuleKey, LinkerModule, ModuleKey::Hash, ModuleKey::Equals> modules;

    void Init(JesusVM& vm) {
        globalVM = &vm; // TODO: better solution. the framework is meant to be usable multiple times in the same runtime
    }

    void Destroy() {
        modulePath.clear();
        modules.clear();
    }

    void AddPath(std::string_view path) {
        while (!path.empty()) {
            auto pos = path.find(':');

            std::string_view currentPath = path.substr(0, pos);

            std::error_code err;
            fs::file_status stat = fs::status(currentPath, err);

            if (!err && fs::exists(stat)) {
                if (fs::is_directory(stat)) {
                    modulePath.push_back(StringPool::Intern(currentPath));
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
        }
    }

    static Module* FindModule(Object* linker, std::string_view name) {

    }

    static Module* LoadModuleWithLinker(Object* linker, std::string_view name) {

    }

    Module* LoadModule(Object* linker, std::string_view name) {
        name = StringPool::Intern(name); // intern string so we don't get fucked by any weird people loading modules

        if (linker != nullptr) {
            return LoadModuleWithLinker(linker, name);
        }

        std::lock_guard<std::mutex> lock(mutex);

        Module* module = FindModule(linker, name);
        if (module != nullptr) {
            return module;
        }

    }

    Class* LoadClass(Object* linker, Module* module, std::string_view name);

    Class* LoadClass(Object* linker, std::string_view qualifiedName);

    Class* LoadPrimitive(std::string_view name);
}
