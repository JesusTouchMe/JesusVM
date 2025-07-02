#ifndef JESUSVM_OBJECT_H
#define JESUSVM_OBJECT_H

#include "JesusVM/JesusNative.h"

#include "JesusVM/heap/gc/CycleCollector.h"

#include "JesusVM/heap/Class.h"

#include "JesusVM/type/Type.h"

#include <atomic>

namespace JesusVM {
    struct Array;

    class ObjectRef;

    class Object {
    friend struct Array;
    friend ObjectRef AllocObject(Class* clas);
    friend ObjectRef AllocArray(Class* clas, Int size);
    // gc
    friend struct GC::Cycle;
    friend void GC::AddPossibleCycleRoot(Object* object);
    friend void GC::MarkRoots();
    friend void GC::ScanRoots();
    friend void GC::CollectRoots();
    friend void GC::FreeCycles();
    friend void GC::SigmaPreparation();
    public:
        using Color = u8;

        static constexpr Color BLACK   = 0; // In use or free
        static constexpr Color GRAY    = 1; // Possible member of cycle
        static constexpr Color WHITE   = 2; // Member of garbage cycle
        static constexpr Color PURPLE  = 3; // Possible root of cycle
        static constexpr Color GREEN   = 4; // Acyclic
        static constexpr Color RED     = 5; // Candidate cycle undergoing sigma-computation
        static constexpr Color ORANGE  = 6; // Candidate cycle awaiting globalEpoch boundary

        Class* getClass();
        i16 getReferenceCount() const;
        i16 getCyclicReferenceCount() const;

        Color getColor() const;

        Array* toArray();

        Int getArrayLength();

        bool isInstance(Class* clas);

        void addReference();
        void addReferenceReal();
        void removeReference();
        void removeReferenceReal(); // this function will LITERALLY std::free(this); if refcount reaches 0. always treat it as such

        u8* getFieldsBuffer();
        const u8* getFieldsBuffer() const;

        template<typename T>
        constexpr T* getArrayElements();

        Bool getBool(Field* field) const;
        Char getChar(Field* field) const;
        Byte getByte(Field* field) const;
        Short getShort(Field* field) const;
        Int getInt(Field* field) const;
        Long getLong(Field* field) const;
        // Float getFloat(Field* field) const;
        // Double getDouble(Field* field) const;
        Handle getHandle(Field* field) const;
        ObjectRef getObject(Field* field) const;
        Object* getObjectWeak(Field* field) const;

        void setBool(Field* field, Bool value);
        void setChar(Field* field, Char value);
        void setByte(Field* field, Byte value);
        void setShort(Field* field, Short value);
        void setInt(Field* field, Int value);
        void setLong(Field* field, Long value);
        // void setFloat(Field* field, Float value) const;
        // void setDouble(Field* field, Double value) const;
        void setHandle(Field* field, Handle value);
        void setObject(Field* field, Object* value);

        template <class Consumer>
        void forEachChild(Consumer consumer) {
            if (mClass->getKind() == ClassKind::ARRAY) {
                if (mClass->mArrayBaseClass->getKind() != ClassKind::PRIMITIVE) {
                    auto elements = getArrayElements<Object*>();
                    for (Int i = 0; i < getArrayLength(); i++) {
                        if (elements[i] != nullptr) {
                            consumer(elements[i]);
                        }
                    }
                }
            } else {
                for (auto& field: mClass->mFields) {
                    if (field.getType().getInternalType() != Type::REFERENCE) {
                        break; // fields are sorted so all references are first. in the weird cases where fields aren't sorted, that's on the guy pushing elements into a private vector (idiot)
                    }

                    Object* value = getObjectWeak(&field);
                    if (value != nullptr) {
                        consumer(value);
                    }
                }
            }
        }

    private:
        explicit Object(Class* clas);

        Class* mClass;

        std::atomic<i16> mRefCount = 1;
        i16 mCyclicRefCount = 0;
        std::atomic<bool> mBuffered = false;
        Color mColor = BLACK;

        void release();
        void finalize();
        void free();

        // gc functions
        void scanBlack();
        void markGray();
        void scanRoot();
        void collectWhite(std::vector<Object*>& currentCycle);
        void cyclicDecrement();
    };

    class ObjectRef {
    public:
        ObjectRef(Object* object) : mObject(object) {
            if (mObject != nullptr)
                mObject->addReference();
        }

        // This constructor doesn't implicitly add its own reference, as it expects the caller to have already added a reference and is simply using this to transfer that.
        ObjectRef(Object* object, bool) : mObject(object) {}

        ObjectRef(std::nullptr_t = nullptr) : mObject(nullptr) {}

        ObjectRef(const ObjectRef& other) : mObject(other.mObject) {
            if (mObject != nullptr) {
                mObject->addReference();
            }
        }

        ObjectRef(ObjectRef&& other) : mObject(other.mObject) {
            other.mObject = nullptr;
        }

        ~ObjectRef() {
            if (mObject != nullptr) {
                mObject->removeReference();
            }
        }

        ObjectRef& operator=(const ObjectRef& other) {
            if (&other != this) {
                mObject = other.mObject;

                if (mObject != nullptr) {
                    mObject->addReference();
                }
            }

            return *this;
        }

        ObjectRef& operator=(ObjectRef&& other) noexcept {
            mObject = other.mObject;
            other.mObject = nullptr;

            return *this;
        }

        ObjectRef& operator=(Object* obj) {
            if (mObject != nullptr) {
                mObject->removeReference();
            }

            mObject = obj;

            if (obj != nullptr) {
                obj->addReference();
            }

            return *this;
        }

        ObjectRef& operator=(std::nullptr_t) {
            if (mObject != nullptr) {
                mObject->removeReference();
            }

            mObject = nullptr;

            return *this;
        }

        Object* get() const {
            return mObject;
        }

        void release() {
            if (mObject != nullptr) {
                mObject->removeReference();
                mObject = nullptr;
            }
        }

        operator Object*() const {
            return mObject;
        }

        operator JObject() const {
            return reinterpret_cast<JObject>(mObject); // this is safe
        }

        Object* operator->() {
            return mObject;
        }

        bool operator==(const ObjectRef& other) const {
            return mObject == other.mObject;
        }

        bool operator==(Object* other) const {
            return mObject == other;
        }

        bool operator!=(const ObjectRef& other) const {
            return *this != other.mObject;
        }

        bool operator!=(Object* other) const {
            return mObject != other;
        }

        operator bool() const {
            return mObject != nullptr;
        }

        bool operator==(std::nullptr_t) const {
            return mObject == nullptr;
        }

        bool operator!=(std::nullptr_t) const {
            return mObject != nullptr;
        }

    protected:
        Object* mObject;
    };

    struct Array {
        Array(Class* clas, Long size); // use the artificial array class. NOT a base class

        Object object;
        const Long size;
    };

    // the famous container_of macro (c++ edition) !!!
    // credit to https://stackoverflow.com/a/40851139

    template <class P, class M>
    constexpr std::size_t _offsetof(const M P::*member) {
        return (std::size_t) &(reinterpret_cast<P*>(0)->*member);
    }

    template<class P, class M>
    constexpr P* _container_of_impl(M* ptr, const M P::*member) {
        return (P*) ((char*) ptr - _offsetof(member));
    }

#   define container_of(ptr, type, member) _container_of_impl(ptr, &type::member)

    template<typename T>
    constexpr T* Object::getArrayElements() {
        return std::launder(reinterpret_cast<T*>(reinterpret_cast<u8*>(this) + sizeof(Array)));
    }

    ObjectRef AllocObject(Class* clas);
    ObjectRef AllocArray(Class* clas, Long size);
    ObjectRef AllocArrayOf(Class* base, Long size);
    ObjectRef AllocPrimitiveArray(u8 typeId, Long size);

    void NullCheck(Object* obj);
}

#endif //JESUSVM_OBJECT_H
