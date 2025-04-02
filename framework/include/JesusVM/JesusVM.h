#ifndef JESUSVM_JESUSVM_H
#define JESUSVM_JESUSVM_H

#include "types.h"

#include "JesusVM/Linker.h"
#include "JesusVM/Module.h"
#include "JesusVM/Options.h"

#include "JesusVM/executors/Thread.h"

#include "JesusVM/heap/Object.h"

#include <memory>
#include <vector>

#define ALIGN(x,a) (((x)+(a)-1)&~((a)-1))

namespace JesusVM {
    struct Context {

    };

    void Init();

    VMContext GetContext();

    std::string_view GetStringData(Object* object);
}

#endif // JESUSVM_JESUSVM_H
