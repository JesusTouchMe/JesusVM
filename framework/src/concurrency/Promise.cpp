// Copyright 2025 JesusTouchMe

#include "JesusVM/concurrency/Promise.h"

#include "JesusVM/util/HandleRegistry.h"

namespace JesusVM {
    Promise::Promise()
        : mFuture(vmHandleRegistry.allocate<Future>()) {}

    Promise::~Promise() {
        vmHandleRegistry.release(mFuture);
    }

    Future* Promise::getFuture() const {
        return mFuture;
    }

    void Promise::setValue(JValue value) {
        mFuture->setValue(value);
    }
}
