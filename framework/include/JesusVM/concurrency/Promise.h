// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_CONCURRENCY_PROMISE_H
#define JESUSVM_CONCURRENCY_PROMISE_H 1

#include "JesusVM/concurrency/Future.h"

namespace JesusVM {
    class Promise {
    public:
        Promise();
        ~Promise();

        Future* getFuture() const;
        void setValue(JValue value);

    private:
        Future* mFuture;
    };
}

#endif // JESUSVM_CONCURRENCY_PROMISE_H
