// Copyright 2025 JesusTouchMe

#include "JesusVM/heap/gc/References.h"

#include <mutex>
#include <utility>

namespace JesusVM::GC {
    struct SpecialObject {
        void* ptr;
        std::function<void(void*)> destructor;

        SpecialObject(void* ptr, std::function<void(void*)> destructor)
            : ptr(ptr)
            , destructor(std::move(destructor)) {}

        ~SpecialObject() {
            if (ptr != nullptr) destructor(ptr);
        }

        void release() {
            if (ptr != nullptr) destructor(ptr);
            ptr = nullptr;
            destructor = nullptr;
        }
    };

    struct ReferenceFrame {
        std::vector<Object*> objects;
        std::vector<size_t> freeList;

        std::vector<SpecialObject> specialObjects;
        std::vector<size_t> specialFreeList;

        explicit ReferenceFrame(size_t initialCapacity = 4) {
            objects.reserve(initialCapacity);
            freeList.reserve(initialCapacity / 2);
        }

        ~ReferenceFrame() {
            for (Object* object : objects) {
                if (object != nullptr) object->removeReference();
            }
        }

        void removeAll() {
            freeList.clear();
            specialFreeList.clear();

            freeList.reserve(objects.size());
            specialFreeList.reserve(specialObjects.size());

            for (size_t i = 0; i < objects.size(); i++) {
                if (objects[i] != nullptr) objects[i]->removeReference();
                freeList.push_back(i);
            }

            for (size_t i = 0; i < specialObjects.size(); i++) {
                if (specialObjects[i].ptr != nullptr) specialObjects[i].release();
                specialFreeList.push_back(i);
            }
        }

        Handle createReference(Object* object) {
            if (!freeList.empty()) {
                size_t index = freeList.back();
                freeList.pop_back();
                objects[index] = object;
                object->addReference();
                return reinterpret_cast<Handle>(index);
            } else {
                objects.push_back(object);
                object->addReference();
                return reinterpret_cast<Handle>(objects.size() - 1);
            }
        }

        Handle createSpecial(void* ptr, std::function<void(void*)> destructor) {
            if (!specialFreeList.empty()) {
                size_t index = specialFreeList.back();
                specialFreeList.pop_back();
                specialObjects[index] = SpecialObject(ptr, std::move(destructor));
                return reinterpret_cast<Handle>(index);
            } else {
                specialObjects.emplace_back(ptr, std::move(destructor));
            }
        }

        void removeReference(Handle reference) {
            size_t index = reinterpret_cast<size_t>(reference);
            if (index >= objects.size() || objects[index] == nullptr) { // retard catcher
                return;
            }

            objects[index]->removeReference();
            objects[index] = nullptr;
            freeList.push_back(index);
        }

        void removeSpecial(Handle reference) {
            size_t index = reinterpret_cast<size_t>(reference);
            if (index >= specialObjects.size() || specialObjects[index].ptr == nullptr) { // retard catcher
                return;
            }

            specialObjects[index].release();
            specialFreeList.push_back(index);
        }
    };

    static ReferenceFrame globalReferences;
    static std::mutex globalMutex;
    static std::mutex globalSpecialMutex;

    static ReferenceFrame globalWeakReferences;
    static std::mutex globalWeakMutex;
    static std::mutex globalWeakSpecialMutex;

    static thread_local std::vector<ReferenceFrame> localReferenceStack;

    Handle CreateGlobalReference(Object* object) {
        std::lock_guard<std::mutex> lock(globalMutex);
        return globalReferences.createReference(object);
    }

    void DeleteGlobalReference(Handle reference) {
        std::lock_guard<std::mutex> lock(globalMutex);
        globalReferences.removeReference(reference);
    }

    Handle CreateGlobalSpecial(void* ptr, std::function<void(void*)> destructor) {
        std::lock_guard<std::mutex> lock(globalSpecialMutex);
        return globalReferences.createSpecial(ptr, std::move(destructor));
    }

    void DeleteGlobalSpecial(Handle object) {
        std::lock_guard<std::mutex> lock(globalSpecialMutex);
        globalReferences.removeSpecial(object);
    }

    Handle CreateGlobalWeakReference(Object* object) {
        std::lock_guard<std::mutex> lock(globalWeakMutex);
        return globalWeakReferences.createReference(object);
    }

    void DeleteGlobalWeakReference(Handle reference) {
        std::lock_guard<std::mutex> lock(globalWeakMutex);
        globalWeakReferences.removeReference(reference);
    }

    Handle CreateGlobalWeakSpecial(void* ptr, std::function<void(void*)> destructor) {
        std::lock_guard<std::mutex> lock(globalWeakSpecialMutex);
        return globalWeakReferences.createSpecial(ptr, std::move(destructor));
    }

    void DeleteGlobalWeakSpecial(Handle object) {
        std::lock_guard<std::mutex> lock(globalWeakSpecialMutex);
        globalWeakReferences.removeSpecial(object);
    }

    VMErr PushLocalFrame(Int initialCapacity) {
        localReferenceStack.emplace_back(initialCapacity);
        return VM_OK;
    }

    VMErr PopLocalFrame() {
        localReferenceStack.pop_back();
        return VM_OK;
    }

    Handle CreateLocalReference(Object* object) {
        return localReferenceStack.back().createReference(object);
    }

    void DeleteLocalReference(Handle reference) {
        localReferenceStack.back().removeReference(reference);
    }

    Handle CreateLocalSpecial(void* ptr, std::function<void(void*)> destructor) {
        return localReferenceStack.back().createSpecial(ptr, std::move(destructor));
    }

    void DeleteLocalSpecial(Handle object) {
        localReferenceStack.back().removeSpecial(object);
    }

    void ReleaseWeakReferences(size_t byteLimit) {
        std::ignore = byteLimit; // TODO: implement byte limits for super optimized heap and gc shit

        std::lock_guard<std::mutex> lock(globalWeakMutex);
        std::lock_guard<std::mutex> lock2(globalWeakSpecialMutex);

        globalWeakReferences.removeAll();
    }
}
