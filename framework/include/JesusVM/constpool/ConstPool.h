#ifndef JESUSVM_CONSTPOOL_H
#define JESUSVM_CONSTPOOL_H

#include "JesusVM/constpool/Constant.h"

#include <memory>

namespace JesusVM {
	class ConstPool {
	public:
		ConstPool(u32 size, moduleweb_constant_info* constants);

		Constant* getGeneric(u32 index);

        template<typename T, Constant::Type Type = T::type>
        T* get(u32 index) {
            Constant* constant = getGeneric(index);

            if (constant == nullptr) {
                return nullptr;
            }

            if (constant->getType() == Type) {
                return static_cast<T*>(constant);
            } else {
                return nullptr;
            }
        }

	private:
		u32 mSize;
        moduleweb_constant_info* mModulewebConstants;
		std::unique_ptr<ConstantPtr[]> mConstants;
	};
}

#endif // JESUSVM_CONSTPOOL_H
