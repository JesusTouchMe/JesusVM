#ifndef JESUSVM_CLASS_H
#define JESUSVM_CLASS_H

#include "types.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/heap/Field.h"
#include "JesusVM/heap/Method.h"

#include "moduleweb/class_info.h"

#include <array>
#include <memory>
#include <string_view>


namespace JesusVM {
    class ConstantName;
	class Module;
    class Object;

    enum class ClassKind {
        REGULAR,
        ARRAY, // artificial
        PRIMITIVE // artificial
    };

    enum class ClassState {
        ERRORED,
        LINKING,
        LINKED,
        INITIALIZED,
    };

    namespace Linker {
        static Class* FindClass(std::unique_lock<std::mutex>&, Module* module, std::string_view name);
        Class* LoadClass(Module* module, std::string_view name);
        Class* LoadClass(std::string_view qualifiedName, Object* linker);
        Class* LoadPrimitive(std::string_view name);
    }

	class Class {
    friend Class* Linker::FindClass(std::unique_lock<std::mutex>&, Module* module, std::string_view name);
    friend Class* Linker::LoadClass(Module* module, std::string_view name);
    friend Class* Linker::LoadClass(std::string_view qualifiedName, Object* linker);
    friend Class* Linker::LoadPrimitive(std::string_view name);
    friend class Object;
	public:
        using Modifiers = u16;

		Class(Module* module, moduleweb_class_info* info);

        bool link();
        bool linkPrimitive(std::string_view name);
        bool linkArray(Class* base, std::string_view name);

        moduleweb_class_info* getInfo() const;
		Module* getModule() const;
        ClassKind getKind() const;
        ClassState getState() const;
		std::string_view getName() const;
        u64 getFieldBufferSize() const;
        u64 getTotalSize() const;

        Field* getField(std::string_view name, std::string_view descriptor);
        Field* getField(ConstantName* name);

        Method* getMethod(std::string_view name, std::string_view descriptor);
        Method* getMethod(ConstantName* name);

        Function* dispatchMethod(Method* method) const;

        bool isAssignableTo(Class* other) const;

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
		std::string_view mName;
        Class* mSuperClass;

        u32 mWaitingThreads;
        Handle mLoadingThread; // used for artificial classes usually

        u64 mMemorySize;

        std::vector<Field> mFields;
        std::vector<Method> mMethods;

        std::vector<Method*> mVTable;

        bool linkCommon();

        void orderFieldBuckets(std::array<FieldBucket, static_cast<u64>(Type::TYPE_COUNT)>& buckets);
        static void orderFieldBucket(FieldBucket* bucket, u32 size, u64& offset);

        union {
            Type mRepresentedPrimitive; // for primitive classes, this will hold a pointer to the underlying type
            Class* mArrayBaseClass{}; // for array classes, this will hold a pointer to the base class
        };
	};
}

#endif // JESUSVM_CLASS_H