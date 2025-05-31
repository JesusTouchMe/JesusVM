#ifndef JESUSVM_FUNCTION_H
#define JESUSVM_FUNCTION_H

#include "types.h"

#include "JesusVM/JesusNative.h"

#include "JesusVM/bytecode/Executor.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/type/Type.h"

#include "moduleweb/code_attribute.h"
#include "moduleweb/function_info.h"
#include "moduleweb/stackmap_attribute.h"

#include <ranges>

namespace JesusVM {
	template <typename ReturnType>
	using NativeFunctionPtr = ReturnType(JESUSVM_CALL *)(VMContext, JValue*);

    class Function;
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
        ~Function();

        moduleweb_function_info* getInfo() const;
		Module* getModule() const;
		const TypeInfo& getReturnType() const;
        const std::vector<TypeInfo>& getArgumentTypes() const;
        u16 getNeededLocalsForArgs() const;
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
        u16 mNeededLocalsForArgs;
		std::string_view mName;
		std::string_view mDescriptor;

        moduleweb_code_attribute mCodeAttribute{};

        template<typename T>
        void processArg(T& arg, Stack::Frame* frame, u16 localIndex, u16 argIndex);

        template<typename Iter, typename T, typename... Rest>
        void processArgs(Stack::Frame* frame, u16& localIndex, u16& argIndex, Iter typeIt, T&& arg, Rest&&... rest);
        inline void processArgs(Stack::Frame*, u16&, u16&, std::vector<TypeInfo>::iterator) {}

        VMContext getNativeContext();
	};

    template<typename R, typename... Args>
    R Function::invoke(Args... args) {
        if (isNative()) return invokeNative<R>(std::forward<Args>(args)...);

        if (isAsync()) {
            Threading::Schedule(this);
            return;
        }

        Executor& executor = Threading::CurrentThread::GetExecutor();
        executor.enterFunction(this);

        Stack::Frame* frame = executor.mFrame;
        u16 localIndex = 0;
        u16 argIndex = 0;

        processArgs(frame, localIndex, argIndex, mArgumentTypes.begin(), args...);

        executor.run();

        if constexpr (!std::is_same_v<R, void>) {
            return static_cast<R>(executor.getReturnValue());
        }
    }

    template<typename R, typename... Args>
    R Function::invokeNative(Args... args) {
        if (!isNative()) {
            std::cout << "error: invokeNative on non-native function\n";
            std::exit(1);
        }

        if (isAsync()) {
            Threading::Schedule(this);
            return;
        }

        if (mCodeAttribute.code == nullptr) {
            Linker::LinkNativeFunction(this);
        }

        auto code = reinterpret_cast<NativeFunctionPtr<R>>(mCodeAttribute.code);

        if constexpr (sizeof...(args) == 0) {
            if constexpr (std::is_same_v<R, void>) {
                code(getNativeContext(), nullptr);
                return;
            } else {
                return code(getNativeContext(), nullptr);
            }
        } else {
            std::array<JValue, sizeof...(args)> runtimeArgs{{args...}};

            if constexpr (std::is_same_v<R, void>) {
                code(getNativeContext(), runtimeArgs.data());
                return;
            } else {
                return code(getNativeContext(), runtimeArgs.data());
            }
        }
    }

    template <typename T>
    void Function::processArg(T& arg, Stack::Frame* frame, u16 localIndex, u16 argIndex) {
        switch (mArgumentTypes[argIndex].getInternalType()) {
            case Type::REFERENCE: {
                if constexpr (std::is_same_v<T, Object*> || std::is_same_v<T, ObjectRef>) {
                    frame->setLocalObject(localIndex, arg);
                } else {
                    std::cerr << "Type mismatch: Expected Object at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::HANDLE: {
                if constexpr (std::is_same_v<T, Handle>) {
                    frame->setLocalHandle(localIndex, arg);
                } else {
                    std::cerr << "Type mismatch: Expected Handle at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::BYTE: {
                if constexpr (std::is_integral_v<T>) {
                    frame->setLocalInt(localIndex, static_cast<Byte>(arg));
                } else {
                    std::cerr << "Type mismatch: Expected Byte at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::SHORT: {
                if constexpr (std::is_integral_v<T>) {
                    frame->setLocalInt(localIndex, static_cast<Short>(arg));
                } else {
                    std::cerr << "Type mismatch: Expected Short at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::INT: {
                if constexpr (std::is_integral_v<T>) {
                    frame->setLocalInt(localIndex, static_cast<Int>(arg));
                } else {
                    std::cerr << "Type mismatch: Expected Int at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::LONG: {
                if constexpr (std::is_same_v<T, Long>) {
                    frame->setLocalLong(localIndex, arg);
                } else {
                    std::cerr << "Type mismatch: Expected Long at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::CHAR: {
                if constexpr (std::is_integral_v<T>) {
                    frame->setLocalInt(localIndex, static_cast<Char>(arg));
                } else {
                    std::cerr << "Type mismatch: Expected Char at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::BOOL: {
                if constexpr (std::is_same_v<T, Bool>) {
                    frame->setLocalInt(localIndex, arg != 0);
                } else {
                    std::cerr << "Type mismatch: Expected Bool at arg " << argIndex << std::endl;
                    std::exit(1);
                }
                break;
            }

            case Type::FLOAT:
            case Type::DOUBLE:
                std::cerr << "Floating point types are unsupported at arg " << argIndex << std::endl;
                std::exit(3);

            default:
                std::cerr << "Unknown type at arg " << argIndex << std::endl;
                std::exit(1);
        }
    }

    template <typename Iter, typename T, typename ...Rest>
    void Function::processArgs(Stack::Frame* frame, u16& localIndex, u16& argIndex, Iter typeIt, T&& arg, Rest&& ...rest) {
        if (typeIt == mArgumentTypes.end()) return;

        processArg(arg, frame, localIndex, argIndex);
        localIndex += typeIt->getSlotCount();
        argIndex++;

        processArgs(frame, localIndex, argIndex, std::next(typeIt), std::forward<Rest>(rest)...);
    }
}

#endif // JESUSVM_FUNCTION_H
