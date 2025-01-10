#pragma once

#include "types.h"

#include "JesusVM/type/FunctionType.h"
#include "JesusVM/type/TypeSystem.h"

namespace JesusVM {
	class Executor;
	class Module;

	class Function {
	friend class Module;
	public:
		using Modifiers = u16;

		Function(TypeSystem& typeSystem, std::string_view descriptor, Modifiers modifiers, u16 localCount, u16 stackSize, u8* entry, u32 bytecodeSize);

		Module* getModule() const;
		FunctionType* getType() const;
		std::string_view getName() const;
		std::string_view getDescriptor() const;
		Modifiers getModifiers() const;
		u16 getLocalCount() const;
		u16 getStackSize() const;
		u8* getEntry() const;
		u32 getBytecodeSize() const;

		void call(Executor& executor);
		
	private:
		Module* mModule;
		
		FunctionType* mType;
		std::string_view mName;
		std::string_view mDescriptor;
		Modifiers mModifiers;

		u16 mLocalCount;
		u16 mStackSize;

		u8* mEntry;
		u32 mBytecodeSize;
	};
}