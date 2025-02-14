#ifndef JESUSVM_FUNCTION_H
#define JESUSVM_FUNCTION_H

#include "types.h"

#include "JesusVM/JesusNative.h"

#include "JesusVM/executors/Executor.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/type/Type.h"

#include "moduleweb/code_attribute.h"
#include "moduleweb/function_info.h"

namespace JesusVM {
	template <typename ReturnType>
	using NativeFunctionPtr = ReturnType(JESUSVM_CALL *)(VMContext, JValue*);

	class Executor;
	class Module;

    namespace Linker {
        void LinkNativeFunction(Function*);
    }

    namespace Preload {
        void PreloadSystemModules();
    }

	class Function {
	friend class Module;
    friend bool ParseFunctionType(Function*);
    friend void Linker::LinkNativeFunction(Function*);
    friend void Preload::PreloadSystemModules();
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

        template<typename R, typename... Args>
        R invoke(Args... args);

        template<typename R, typename... Args>
        R invokeNative(Args... args);

	private:
        moduleweb_function_info* mInfo;

		Module* mModule;
		
		TypeInfo mReturnType;
        std::vector<TypeInfo> mArgumentTypes;
		std::string_view mName;
		std::string_view mDescriptor;
		Modifiers mModifiers;

        moduleweb_code_attribute mCodeAttribute;

        VMContext getNativeContext();
        JesusVM& getVM();
	};

    template<typename T>
    void AddArgument(std::vector<i32>& args, T value) {
        if constexpr (sizeof(T) == 8) {
            i64 val = reinterpret_cast<i64>(value);
            args.push_back(static_cast<i32>(val >> 32));
            args.push_back(static_cast<i32>(val & 0xFFFFFFFF));
        } else {
            args.push_back(static_cast<i32>(value));
        }
    }

    template<typename R, typename... Args>
    R Function::invoke(Args... args) {
        if (isNative()) return invokeNative<R>(args...);

        if (isAsync()) {
            Threading::LaunchThread(this);
            return;
        }

        std::vector<i32> runtimeArgs;
        runtimeArgs.reserve(mArgumentTypes.size() * 2); // reserve for worst case scenario

        (AddArgument(runtimeArgs, args), ...);

        Executor executor(getVM());
        executor.enterFunction(this);
        executor.run();

        if constexpr (!std::is_same_v<R, void>) {
            return static_cast<R>(executor.getReturnValue());
        }
    }

    template<typename R, typename... Args>
    R Function::invokeNative(Args... args) {
        if (!isNative()) {
            if constexpr (std::is_same_v<R, void>) {
                return;
            } else {
                return static_cast<R>(0);
            }
        }

        std::array<JValue, sizeof...(args)> runtimeArgs{{args...}};

        if (mCodeAttribute.code == nullptr) {
            Linker::LinkNativeFunction(this);
        }

        auto code = reinterpret_cast<NativeFunctionPtr<R>>(mCodeAttribute.code);

        if constexpr (std::is_same_v<R, void>) {
            code(getNativeContext(), runtimeArgs.data());
            return;
        } else {
            return code(getNativeContext(), runtimeArgs.data());
        }
    }
}

#endif // JESUSVM_FUNCTION_H
