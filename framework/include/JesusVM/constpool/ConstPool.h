#pragma once

#include "JesusVM/constpool/Constant.h"

#include <memory>

namespace JesusVM {
	class ConstPool {
	public:
		ConstPool(u32 size);

		Constant* get(u32 index) const;
		
		void set(u32 index, ConstantPtr value);

	private:
		u32 mSize;
		std::unique_ptr<ConstantPtr[]> mConstants;
	};
}