#include "JesusVM/Linker.h"

#include <unordered_map>

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

    std::unordered_map<std::string_view, std::unique_ptr<Module>> modules;

    void Init() {

    }

    void Destroy();

    void AddPath(std::string_view path);

    Module* LoadModule(Object* linker, std::string_view name);

    Class* LoadClass(Object* linker, Module* module, std::string_view name);

    Class* LoadClass(Object* linker, std::string_view qualifiedName);

    Class* LoadPrimitive(std::string_view name);
}
