#include "JesusVM/constpool/ConstantFunc.h"

namespace JesusVM {
    ConstantFunc::ConstantFunc(Function* func)
        : Constant(Type::FUNCTION)
        , mFunc(func) {}

    Function* ConstantFunc::getFunction() {
        return mFunc;
    }

    void ConstantFunc::addTo(Stack::Frame* frame) {
        frame->pushLong(reinterpret_cast<i64>(mFunc));
    }
}