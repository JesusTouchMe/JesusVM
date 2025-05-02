// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_GC_DAEMON_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_GC_DAEMON_H

namespace JesusVM::GC::Daemon {
    void Launch();

    // Call every epoch after the mutation buffers have been processed
    void BeginCollection();
}

#endif //JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_GC_DAEMON_H
