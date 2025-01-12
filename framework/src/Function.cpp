#include "JesusVM/Function.h"

namespace JesusVM {
	Function::Function(TypeSystem& typeSystem, std::string_view descriptor, Modifiers modifiers, u16 localCount, u16 stackSize, u8* entry, u32 bytecodeSize)
		: mModule(nullptr)
		, mDescriptor(descriptor)
		, mModifiers(modifiers)
		, mLocalCount(localCount)
		, mStackSize(stackSize)
		, mEntry(entry)
		, mBytecodeSize(bytecodeSize) {
		u64 index = mDescriptor.find('(');

		if (index != std::string_view::npos) {
			std::string_view typeDesc = mDescriptor.substr(index);

			Type* type = typeSystem.getType(typeDesc);

			mType = dynamic_cast<FunctionType*>(type);
			mName = descriptor.substr(0, index);
		}
	}

	Module* Function::getModule() const {
		return mModule;
	}

	FunctionType* Function::getType() const {
		return mType;
	}

	std::string_view Function::getName() const {
		return mName;
	}
	
	std::string_view Function::getDescriptor() const {
		return mDescriptor;
	}
	
	Function::Modifiers Function::getModifiers() const {
		return mModifiers;
	}
	
	u16 Function::getLocalCount() const {
		return mLocalCount;
	}
	
	u16 Function::getStackSize() const {
		return mStackSize;
	}
	
	u8* Function::getEntry() const {
		return mEntry;
	}
	
	u32 Function::getBytecodeSize() const {
		return mBytecodeSize;
	}
}