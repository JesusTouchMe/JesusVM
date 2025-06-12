// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_METHOD_H
#define JESUSVM_METHOD_H 1

#include "JesusVM/type/Type.h"

#include "moduleweb/method_info.h"

#include <string_view>

namespace JesusVM {
    class Class;
    class Function;

    class Method {
    friend class Class;
    friend bool ParseFunctionType<Method>(Method*);
    public:
        Method(Class* clas, u32 index);

        Class* getClass() const;
        moduleweb_method_info* getInfo() const;
        const TypeInfo& getReturnType() const;
        const std::vector<TypeInfo>& getArgumentTypes() const;
        u16 getModifiers() const;
        std::string_view getName() const;
        std::string_view getDescriptor() const;
        u32 getIndex() const;
        Function* getFunction() const;

        bool isPublic() const;
        bool isPrivate() const;
        bool isProtected() const;
        bool isAbstract() const;
        bool isFinal() const;

    private:
        Class* mClass;
        u32 mIndex;

        TypeInfo mReturnType;
        std::vector<TypeInfo> mArgumentTypes;
		std::string_view mName;
		std::string_view mDescriptor;

        Function* mFunction; // the actual callee. methods just exist to be in the vtable

        u32 mVTableIndex;
    };
}

#endif //JESUSVM_METHOD_H