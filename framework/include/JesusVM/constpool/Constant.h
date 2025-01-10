#pragma once

#include "types.h"

#include <memory>

namespace JesusVM {
	class Constant {
	public:
	};

	using ConstantPtr = std::unique_ptr<Constant>;
}