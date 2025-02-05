#ifndef JESUSVM_FUNCTION_H
#define JESUSVM_FUNCTION_H

#include "types.h"

#include "JesusVM/JesusNative.h"

#include "JesusVM/type/Type.h"

#include "moduleweb/function_info.h"

namespace JesusVM {
	template <typename ReturnType>
	using NativeFunctionPtr = ReturnType(NATIVECALL *)(VMContext, JValue*);

	class Executor;
	class Module;

	class Function {
	friend class Module;
    friend bool ParseFunctionType(Function*);
	public:
		using Modifiers = u16;

		Function(Module* module, moduleweb_function_info* info);

        moduleweb_function_info* getInfo() const;
		Module* getModule() const;
		const TypeInfo& getReturnType() const;
        const std::vector<TypeInfo>& getArgumentTypes();
		std::string_view getName() const;
		std::string_view getDescriptor() const;
		Modifiers getModifiers() const;
		u16 getLocalCount() const;
		u16 getStackSize() const;
		u8* getEntry() const;
		u32 getBytecodeSize() const;

        bool isPublic() const;
        bool isPrivate() const;
        bool isPure() const;
        bool isAsync() const;
        bool isNative() const;

	private:
        moduleweb_function_info* mInfo;

		Module* mModule;
		
		TypeInfo mReturnType;
        std::vector<TypeInfo> mArgumentTypes;
		std::string_view mName;
		std::string_view mDescriptor;
		Modifiers mModifiers;

		u16 mLocalCount;
		u16 mStackSize;

		u8* mEntry;
		u32 mBytecodeSize;
	};
}

#endif // JESUSVM_FUNCTION_H
