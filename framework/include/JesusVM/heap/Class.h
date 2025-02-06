#ifndef JESUSVM_CLASS_H
#define JESUSVM_CLASS_H

#include "types.h"

#include "JesusVM/heap/Field.h"

#include "moduleweb/class_info.h"

#include <array>
#include <memory>
#include <string_view>

namespace JesusVM {
	class Module;
    class Object;

    enum class ClassKind {
        REGULAR,
        ARRAY, // artificial
        PRIMITIVE // artificial
    };

    enum class ClassState {
        ERRORED,
        LINKED,
        INITIALIZED,
    };

	class Class {
	public:
        using Modifiers = u16;

		explicit Class(Module* module);

        bool link(moduleweb_class_info* info);
        bool linkPrimitive(std::string_view name);
        bool linkArray(Class* base, std::string_view name);

        moduleweb_class_info* getInfo() const;
		Module* getModule() const;
		std::string_view getName() const;

        bool isPublic() const;
        bool isPrivate() const;
        bool isAbstract() const;
        bool isFinal() const;

	private:
        struct FieldBucket {
            std::vector<Field*> fields;
        };

        moduleweb_class_info* mInfo;

        ClassKind mKind;
        ClassState mState;
        Modifiers mModifiers;

		Module* mModule;
        Object* mLinker; // nullptr means bootstrap linker which does not have any virtual representation

		std::string_view mName;

        Class* mSuperClass;

        u64 mMemorySize;

        std::vector<Field> mFields;

        bool linkCommon();

        void orderFieldBuckets(std::array<FieldBucket, static_cast<u64>(Type::TYPE_COUNT)>& buckets);
        static void orderFieldBucket(FieldBucket* bucket, u32 size, u64& offset);

        union {
            TypeInfo mRepresentedPrimitive; // for primitive classes, this will hold a pointer to the underlying type
            Class* mArrayBaseClass{}; // for array classes, this will hold a pointer to the base class
        };
	};
}

#endif // JESUSVM_CLASS_H