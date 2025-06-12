// Copyright 2025 JesusTouchMe

#include "JesusVM/constpool/ConstantFunc.h"

#include "JesusVM/heap/Class.h"
#include "JesusVM/heap/Method.h"

#include "JesusVM/Module.h"

namespace JesusVM {
    Method::Method(Class* clas, u32 index)
        : mClass(clas)
        , mIndex(index) {
        moduleweb_method_info* info = &clas->getInfo()->methods[index];

        auto name = clas->getModule()->getConstPool().get<ConstantName>(info->name_index);

        mName = name->getName();
        mDescriptor = name->getDescriptor();

        if (ParseFunctionType(this)) {
            std::cout << "Bad function descriptor\n";
            std::exit(1);
        }

        if (isAbstract()) {
            initAbstract();
            return;
        }

        auto func = clas->getModule()->getConstPool().get<ConstantFunc>(info->function_index);
        mFunction = func->getFunction();

        //TODO: validate the function types match
    }

    Class* Method::getClass() const {
        return mClass;
    }

    moduleweb_method_info* Method::getInfo() const {
        return &mClass->getInfo()->methods[mIndex];
    }

    const TypeInfo& Method::getReturnType() const {
        return mReturnType;
    }

    const std::vector<TypeInfo>& Method::getArgumentTypes() const {
        return mArgumentTypes;
    }

    u16 Method::getModifiers() const {
        return getInfo()->modifiers;
    }

    std::string_view Method::getName() const {
        return mName;
    }

    std::string_view Method::getDescriptor() const {
        return mDescriptor;
    }

    u32 Method::getIndex() const {
        return mIndex;
    }

    Function* Method::getFunction() const {
        return mFunction;
    }

    bool Method::isPublic() const {
        return getModifiers() & MODULEWEB_METHOD_MODIFIER_PUBLIC;
    }

    bool Method::isPrivate() const {
        return getModifiers() & MODULEWEB_METHOD_MODIFIER_PRIVATE;
    }

    bool Method::isProtected() const {
        return getModifiers() & MODULEWEB_METHOD_MODIFIER_PROTECTED;
    }

    bool Method::isAbstract() const {
        return getModifiers() & MODULEWEB_METHOD_MODIFIER_ABSTRACT;
    }

    bool Method::isFinal() const {
        return getModifiers() & MODULEWEB_METHOD_MODIFIER_FINAL;
    }
}
