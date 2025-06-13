#include "JesusVM/Linker.h"

#include "JesusVM/heap/Object.h"

#include "JesusVM/runtime/vm/System.h"

#include <cstdlib>
#include <cstring>
#include <format>

namespace JesusVM {
    Class* Object::getClass() {
        return mClass;
    }

    i16 Object::getReferenceCount() const {
        return mRefCount;
    }

    i16 Object::getCyclicReferenceCount() const {
        return mCyclicRefCount;
    }

    Object::Color Object::getColor() const {
        return mColor;
    }
    
    Array* Object::toArray() {
        return container_of(this, Array, object);
    }

    Int Object::getArrayLength() {
        return toArray()->size;
    }

    bool Object::isInstance(Class* clas) {
        return mClass->isAssignableTo(clas);
    }

    void Object::addReference() {
        auto& buffer = Threading::CurrentThread::GetMutationBuffer();
        buffer.addReference(this);
    }

    void Object::addReferenceReal() {
        mRefCount += 1;
        scanBlack();
    }

    void Object::removeReference() {
        auto& buffer = Threading::CurrentThread::GetMutationBuffer();
        buffer.removeReference(this);
    }

    void Object::removeReferenceReal() {
        mRefCount -= 1;

        if (mRefCount <= 0) {
            release();
        } else {
            GC::AddPossibleCycleRoot(this);
        }
    }

    u8* Object::getFieldsBuffer() {
        return reinterpret_cast<u8*>(std::launder(this + 1));
    }

    const u8* Object::getFieldsBuffer() const {
        return reinterpret_cast<const u8*>(std::launder(this + 1));
    }

    Bool Object::getBool(Field* field) const {
        const u8* fields = getFieldsBuffer();
        return fields[field->getMemoryOffset()] != 0;
    }

    Char Object::getChar(Field* field) const {
        const u8* fields = getFieldsBuffer();
        return static_cast<Char>(fields[field->getMemoryOffset()]);
    }

    Byte Object::getByte(Field* field) const {
        const u8* fields = getFieldsBuffer();
        return static_cast<Byte>(fields[field->getMemoryOffset()]);
    }

    Short Object::getShort(Field* field) const {
        const u8* fields = getFieldsBuffer();
        Short value;
        std::memcpy(&value, fields + field->getMemoryOffset(), sizeof(Short));
        return value;
    }

    Int Object::getInt(Field* field) const {
        const u8* fields = getFieldsBuffer();
        Int value;
        std::memcpy(&value, fields + field->getMemoryOffset(), sizeof(Int));
        return value;
    }

    Long Object::getLong(Field* field) const {
        const u8* fields = getFieldsBuffer();
        Long value;
        std::memcpy(&value, fields + field->getMemoryOffset(), sizeof(Long));
        return value;
    }

    Handle Object::getHandle(Field* field) const {
        const u8* fields = getFieldsBuffer();
        Handle value;
        std::memcpy(&value, fields + field->getMemoryOffset(), sizeof(Handle));
        return value;
    }

    ObjectRef Object::getObject(Field* field) const {
        return getObjectWeak(field);
    }

    Object* Object::getObjectWeak(Field* field) const {
        const u8* fields = getFieldsBuffer();
        Object* value;
        std::memcpy(&value, fields + field->getMemoryOffset(), sizeof(Object*));
        return value;
    }

    void Object::setBool(Field* field, Bool value) {
        u8* fields = getFieldsBuffer();
        fields[field->getMemoryOffset()] = value;
    }

    void Object::setChar(Field* field, Char value) {
        u8* fields = getFieldsBuffer();
        fields[field->getMemoryOffset()] = static_cast<u8>(value);
    }

    void Object::setByte(Field* field, Byte value) {
        u8* fields = getFieldsBuffer();
        fields[field->getMemoryOffset()] = static_cast<u8>(value);
    }

    void Object::setShort(Field* field, Short value) {
        u8* fields = getFieldsBuffer();
        std::memcpy(fields + field->getMemoryOffset(), &value, sizeof(value));
    }

    void Object::setInt(Field* field, Int value) {
        u8* fields = getFieldsBuffer();
        std::memcpy(fields + field->getMemoryOffset(), &value, sizeof(value));
    }

    void Object::setLong(Field* field, Long value) {
        u8* fields = getFieldsBuffer();
        std::memcpy(fields + field->getMemoryOffset(), &value, sizeof(value));
    }

    void Object::setHandle(Field* field, Handle value) {
        u8* fields = getFieldsBuffer();
        std::memcpy(fields + field->getMemoryOffset(), &value, sizeof(value));
    }

    void Object::setObject(Field* field, Object* value) {
        u8* fields = getFieldsBuffer();
        Object* old = getObjectWeak(field);

        if (old != nullptr) {
            old->removeReference();
        }

        std::memcpy(fields + field->getMemoryOffset(), &value, sizeof(Object*)); // clion said sizeof(value) was suspicious here. i got scared

        if (value != nullptr) {
            value->addReference();
        }
    }

    Object::Object(Class* clas)
        : mClass(clas) {}

    void Object::release() {
        std::cout << "Releasing instance of " << mClass->getName() << "\n";

        finalize();

        forEachChild([](Object* object) {
            object->removeReferenceReal();
        });

        mColor = BLACK;

        if (!mBuffered) {
            free();
        }
    }

    void Object::finalize() {
        std::cout << "Finalizing instance of " << mClass->getName() << "\n";
    }

    void Object::free() {
        std::cout << "Freeing instance of " << mClass->getName() << "\n";

        if (mClass->getKind() == ClassKind::ARRAY) {
            toArray()->~Array(); // :tongue:

#ifdef PLATFORM_WINDOWS
            _aligned_free(toArray());
#else
            std::free(toArray());
#endif
        } else {
            this->~Object(); // :tongue:

#ifdef PLATFORM_WINDOWS
            _aligned_free(this);
#else
            std::free(this);
#endif
        }
    }

    void Object::scanBlack() {
        if (mColor != BLACK) {
            mColor = BLACK;
            forEachChild([](Object* object) {
                object->scanBlack();
            });
        }
    }

    void Object::markGray() {
        if (mColor != GRAY) {
            mColor = GRAY;
            mCyclicRefCount = mRefCount;
            forEachChild([](Object* object) {
                object->mCyclicRefCount -= 1;
                object->markGray();
            });
        } else if (mCyclicRefCount > 0) {
            mCyclicRefCount -= 1;
        }
    }

    void Object::scanRoot() {
        if (mColor == GRAY) {
            if (mCyclicRefCount == 0) {
                mColor = WHITE;
                forEachChild([](Object* object) {
                    object->scanRoot();
                });
            } else {
                mColor = BLACK;
            }
        }
    }

    void Object::collectWhite(std::vector<Object*>& currentCycle) {
        if (mColor == WHITE) {
            mColor = ORANGE;
            mBuffered = true;
            currentCycle.push_back(this);

            forEachChild([&currentCycle](Object* object) {
                object->collectWhite(currentCycle);
            });
        }
    }

    void Object::cyclicDecrement() {
        if (mColor != RED) {
            if (mColor == ORANGE) {
                mRefCount -= 1;
                mCyclicRefCount -= 1;
            } else {
                removeReferenceReal();
            }
        }
    }

    Array::Array(Class* clas, Long size)
        : object(clas)
        , size(size) {}

    ObjectRef AllocObject(Class* clas) {
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(clas->getTotalSize(), alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), clas->getTotalSize()));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }

        std::memset(memory, 0, clas->getTotalSize());

        auto object = new(memory) Object(clas);
        return {object, false};
    }

    ObjectRef AllocArray(Class* clas, Long size) {
        u64 totalSize = sizeof(Array) + sizeof(Object*) * size;
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(totalSize, alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), totalSize));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }

        std::memset(memory, 0, totalSize);

        auto array = new(memory) Array(clas, size);

        return {&array->object, false};
    }

    ObjectRef AllocArrayOf(Class* base, Int size) {
        std::string name = std::format("[R{};", base->getName());
        Class* clas = Linker::LoadClass(base->getModule(), name);
        return AllocArray(clas, size);
    }

    ObjectRef AllocPrimitiveArray(u8 typeId, Int size) {
        Type type = CodeTypeToType(typeId);

        u64 totalSize = sizeof(Array) + GetTypeSize(type) * size;
#ifdef PLATFORM_WINDOWS
        u8* memory = static_cast<u8*>(_aligned_malloc(totalSize, alignof(Array)));
#else
        u8* memory = static_cast<u8*>(std::aligned_alloc(alignof(Array), totalSize));
#endif

        if (memory == nullptr) {
            return nullptr; //TODO: out-of-memory error
        }

        std::memset(memory, 0, totalSize);

        Class* clas;

        switch (typeId) {
            case T_BYTE:
                clas = rt::vm::System::byteArray;
                break;
            case T_SHORT:
                clas = rt::vm::System::shortArray;
                break;
            case T_INT:
                clas = rt::vm::System::intArray;
                break;
            case T_LONG:
                clas = rt::vm::System::longArray;
                break;
            case T_CHAR:
                clas = rt::vm::System::charArray;
                break;
            case T_FLOAT:
                clas = rt::vm::System::floatArray;
                break;
            case T_DOUBLE:
                clas = rt::vm::System::doubleArray;
                break;
            case T_BOOL:
                clas = rt::vm::System::boolArray;
                break;

            default:
                // it will never reach here since CodeTypeToType() error and crash way before
                std::exit(1);
        }

        if (clas == nullptr) {
            std::cout << "error: what?\n"; // i really hope this is never reached
            std::exit(1);
        }

        auto array = new(memory) Array(clas, size);

        return {&array->object, false};
    }

    void NullCheck(Object* obj) {
        if (obj == nullptr) {
            std::cout << "error: null reference\n";
            std::exit(1);
        }
    }

}