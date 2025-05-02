// Copyright 2025 JesusTouchMe

// The Real-Time Cycle Garbage Collection algorithm
// https://pages.cs.wisc.edu/~cymen/misc/interests/Bacon01Concurrent.pdf

#ifndef JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_CYCLECOLLECTOR_H
#define JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_CYCLECOLLECTOR_H 1

namespace JesusVM {
    class Object;
}

// We need to expose every function of the collector so Object can make them friends
namespace JesusVM::GC {
    struct Cycle;

    void AddPossibleCycleRoot(Object* object);

    void MarkRoots();

    void ScanRoots();

    void CollectRoots();

    void FreeCycles();

    void CollectCycles();

    void SigmaPreparation();

    void ProcessCycles();
}

#endif // JESUSVM_FRAMEWORK_INCLUDE_JESUSVM_HEAP_GC_CYCLECOLLECTOR_H
