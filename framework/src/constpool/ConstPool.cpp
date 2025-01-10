#include "JesusVM/constpool/ConstPool.h"

namespace JesusVM {
	ConstPool::ConstPool(u32 size)
		: mSize(size)
		, mConstants(std::make_unique<ConstantPtr[]>(size)) { }

	Constant* ConstPool::get(u32 index) const {
		if (index >= mSize) {
			return nullptr;
		}

		return mConstants[index].get();
	}

	void ConstPool::set(u32 index, ConstantPtr value) {
		if (index >= mSize) {
			return;
		}

		mConstants[index] = std::move(value);
	}
}