#ifndef JESUSVM_EXECUTOR_H
#define JESUSVM_EXECUTOR_H

#include "types.h"

#include "JesusVM/JesusNative.h"
#include "JesusVM/Stack.h"

#include "moduleweb/code_attribute.h"

#include <array>

namespace JesusVM {
	class Executor {
    friend class Function;
	public:
		class WideGuard {
		public:
			explicit WideGuard(Executor& executor)
				: mExecutor(executor) {}

			~WideGuard() {
				mExecutor.mWide = false;
			}

			operator bool() const {
				return mExecutor.mWide;
			}

		private:
			Executor& mExecutor;
		};

        using OpHandler = void(*)(Executor&);
        using DispatchTable = std::array<OpHandler, 256>;

		Executor();

		Stack::Frame* getFrame() const;
		WideGuard getWideGuard();
        JValue getReturnValue() const;

		JValue& returnValue();

		void setWide(bool value);

		void branch(i16 branch);

        void run(); // run until return

		void executeInstruction();

		void enterFunction(Function* function);
        void leaveFunction();

		u8 fetch();
		u16 fetchShort();
		u32 fetchInt();
		u64 fetchLong();

        static void InitDispatch();

	private:
		Stack mStack;
		Stack::Frame* mFrame;

        u8* mCode;
		u32 mPC;

		bool mWide;

        i32 mReturnDepth;
        JValue mReturnValue;

        static DispatchTable mDispatchTable;
        static std::array<DispatchTable, 16> mExtDispatchTables;

		static void ThrowFetchOutOfBounds();
		static void ThrowInvalidOpcode(u32 opcode);
	};
}

#endif // JESUSVM_EXECUTOR_H