#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
	ConstPool::ConstPool(JesusVM& vm, u32 size, moduleweb_constant_info* constants)
		: mVM(vm)
        , mSize(size)
        , mModulewebConstants(constants)
		, mConstants(std::make_unique<ConstantPtr[]>(size)) { }

	Constant* ConstPool::getGeneric(u32 index) const {
		if (index >= mSize) {
			return nullptr;
		}

		auto& constant = mConstants[index];

        if (constant == nullptr) {
            constant = Constant::Create(mVM, &mModulewebConstants[index]);
        }

        return constant.get();
	}
}