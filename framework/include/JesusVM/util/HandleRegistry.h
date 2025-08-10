// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_UTIL_HANDLEREGISTRY_H
#define JESUSVM_UTIL_HANDLEREGISTRY_H 1

#include "JesusVM/JesusNative.h"

#include <memory>
#include <mutex>
#include <typeindex>
#include <unordered_map>

namespace JesusVM {
    class HandleRegistry {
    public:
        template<class T, class... Args>
        T* allocate(Args&&... args) {
            auto obj = std::make_unique<Entry<T>>(std::forward<Args>(args)...);
            T* handle = &obj->object;

            std::lock_guard<std::mutex> lock(mMutex);
            mHandles.emplace(handle, std::move(obj));

            return handle;
        }

        void release(Handle handle) {
            if (handle == nullptr) return;

            std::lock_guard<std::mutex> lock(mMutex);
            mHandles.erase(handle);
        }

        template<class T>
        T* get(Handle handle) const {
            std::lock_guard<std::mutex> lock(mMutex);

            auto it = mHandles.find(handle);
            if (it == mHandles.end()) return nullptr;
            if (it->second->type() != typeid(T)) return nullptr;

            return &static_cast<Entry<T>*>(it->second.get())->object;
        }

        bool exists(Handle handle) const {
            std::lock_guard<std::mutex> lock(mMutex);
            return mHandles.contains(handle);
        }

    private:
        struct EntryBase {
            virtual ~EntryBase() = default;
            virtual std::type_index type() const = 0;
        };

        template<class T>
        struct Entry : EntryBase {
            template<typename... Args>
            explicit Entry(Args&&... args) : object(std::forward<Args>(args)...) {}
            std::type_index type() const override { return typeid(T); }

            T object;
        };

        struct PtrHash {
            using is_transparent = void;

            size_t operator()(const std::unique_ptr<EntryBase>& p) const noexcept {
                return std::hash<void*>()(p.get());
            }
        };

        struct PtrEqual {
            using is_transparent = void;

            bool operator()(const std::unique_ptr<EntryBase>& a, const std::unique_ptr<EntryBase>& b) const noexcept {
                return a.get() == b.get();
            }

            bool operator()(const std::unique_ptr<EntryBase>& a, Handle raw) const noexcept {
                return a.get() == raw;
            }

            bool operator()(Handle raw, const std::unique_ptr<EntryBase>& a) const noexcept {
                return a.get() == raw;
            }
        };

        mutable std::mutex mMutex;
        std::unordered_map<Handle, std::unique_ptr<EntryBase>, PtrHash, PtrEqual> mHandles;
    };

    extern HandleRegistry vmHandleRegistry;
}

#endif // JESUSVM_UTIL_HANDLEREGISTRY_H
