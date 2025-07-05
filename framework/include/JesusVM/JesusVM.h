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
    void Init();

    VMContext* GetContext();

    std::string_view GetStringData(Object* object);

    void Trap(Object* message, std::string file, Int line, Int column);
    void AddTrapHook(void (*hook)(VMContext*, TrapInfo*));
}

#endif // JESUSVM_JESUSVM_H
