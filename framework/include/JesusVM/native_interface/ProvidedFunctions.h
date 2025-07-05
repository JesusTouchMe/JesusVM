// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_NATIVE_PROVIDED_FUNCTIONS_H
#define JESUSVM_NATIVE_PROVIDED_FUNCTIONS_H 1

#include "JesusVM/JesusNative.h"

#undef GetClassName

namespace JesusVM::NativeInterface {
    void Prepare(_JesusVMNativeInterface* interface);

    // Module operations

    ModuleRef GetModule(VMContext* ctx, const char* name);
    ModuleRef CreateModuleA(VMContext* ctx, const char* name, const char* module_path, Int mode);
    ModuleRef CreateModuleU(VMContext* ctx, const char* name, const char* module_path, Int mode);
    ModuleRef CreateModuleMem(VMContext* ctx, const char* name, JObject linker, const Byte* data, Long data_length, Int mode);

    // Error operations

    void Trap(VMContext* ctx, const char* message);
    void AddTrapHook(VMContext* ctx, void hook(VMContext* ctx, TrapInfo* info));

    // References

    Handle CreateGlobalRef(VMContext* ctx, JObject obj);
    void DeleteGlobalRef(VMContext* ctx, Handle ref);

    VMErr PushLocalFrame(VMContext* ctx, Int initial_capacity);
    VMErr PopLocalFrame(VMContext* ctx);
    Handle CreateLocalRef(VMContext* ctx, JObject obj);
    void DeleteLocalRef(VMContext* ctx, Handle ref);

    // Global var operations

    GlobalVarRef GetGlobalVar(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor);

    Bool GetBoolGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Byte GetByteGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Short GetShortGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Int GetIntGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Long GetLongGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Char GetCharGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Float GetFloatGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Double GetDoubleGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    JObject GetObjectGlobalVar(VMContext* ctx, GlobalVarRef global_var);
    Handle GetHandleGlobalVar(VMContext* ctx, GlobalVarRef global_var);

    Bool SetBoolGlobalVar(VMContext* ctx, GlobalVarRef global_var, Bool new_value);
    Byte SetByteGlobalVar(VMContext* ctx, GlobalVarRef global_var, Byte new_value);
    Short SetShortGlobalVar(VMContext* ctx, GlobalVarRef global_var, Short new_value);
    Int SetIntGlobalVar(VMContext* ctx, GlobalVarRef global_var, Int new_value);
    Long SetLongGlobalVar(VMContext* ctx, GlobalVarRef global_var, Long new_value);
    Char SetCharGlobalVar(VMContext* ctx, GlobalVarRef global_var, Char new_value);
    Float SetFloatGlobalVar(VMContext* ctx, GlobalVarRef global_var, Float new_value);
    Double SetDoubleGlobalVar(VMContext* ctx, GlobalVarRef global_var, Double new_value);
    JObject SetObjectGlobalVar(VMContext* ctx, GlobalVarRef global_var, JObject new_value);
    Handle SetHandleGlobalVar(VMContext* ctx, GlobalVarRef global_var, Handle new_value);

    // Function operations

    FunctionRef GetFunction(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor);

    void CallVoidFunction(VMContext* ctx, FunctionRef function, ...);
    void CallVoidFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    void CallVoidFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Bool CallBoolFunction(VMContext* ctx, FunctionRef function, ...);
    Bool CallBoolFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Bool CallBoolFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Byte CallByteFunction(VMContext* ctx, FunctionRef function, ...);
    Byte CallByteFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Byte CallByteFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Short CallShortFunction(VMContext* ctx, FunctionRef function, ...);
    Short CallShortFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Short CallShortFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Int CallIntFunction(VMContext* ctx, FunctionRef function, ...);
    Int CallIntFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Int CallIntFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Long CallLongFunction(VMContext* ctx, FunctionRef function, ...);
    Long CallLongFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Long CallLongFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Char CallCharFunction(VMContext* ctx, FunctionRef function, ...);
    Char CallCharFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Char CallCharFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Float CallFloatFunction(VMContext* ctx, FunctionRef function, ...);
    Float CallFloatFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Float CallFloatFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Double CallDoubleFunction(VMContext* ctx, FunctionRef function, ...);
    Double CallDoubleFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Double CallDoubleFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    JObject CallObjectFunction(VMContext* ctx, FunctionRef function, ...);
    JObject CallObjectFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    JObject CallObjectFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    Handle CallHandleFunction(VMContext* ctx, FunctionRef function, ...);
    Handle CallHandleFunctionA(VMContext* ctx, FunctionRef function, const JValue* args);
    Handle CallHandleFunctionV(VMContext* ctx, FunctionRef function, va_list args);

    // Class operations

    ClassRef GetClass(VMContext* ctx, ModuleRef module, const char* name);
    ClassRef GetSuperClass(VMContext* ctx, ClassRef clas);
    const char* GetClassName(VMContext* ctx, ClassRef clas);
    JObject GetClassLinker(VMContext* ctx, ClassRef clas);
    Bool IsAssignable(VMContext* ctx, ClassRef from, ClassRef to);

    ClassRef CreateArtificialClass(VMContext* ctx, ModuleRef module, ArtificialClass* clas);

    // Object operations

    JObject AllocObject(VMContext* ctx, ClassRef clas);
    JObject NewObject(VMContext* ctx, ClassRef clas, FunctionRef constructor, ...);
    JObject NewObjectA(VMContext* ctx, ClassRef clas, FunctionRef constructor, const JValue* args);
    JObject NewObjectV(VMContext* ctx, ClassRef clas, FunctionRef constructor, va_list args);
    ClassRef GetObjectClass(VMContext* ctx, JObject obj);
    Bool IsInstance(VMContext* ctx, JObject obj, ClassRef clas);

    // Field operations

    FieldRef GetField(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor);

    Bool GetBoolField(VMContext* ctx, JObject obj, FieldRef field);
    Byte GetByteField(VMContext* ctx, JObject obj, FieldRef field);
    Short GetShortField(VMContext* ctx, JObject obj, FieldRef field);
    Int GetIntField(VMContext* ctx, JObject obj, FieldRef field);
    Long GetLongField(VMContext* ctx, JObject obj, FieldRef field);
    Char GetCharField(VMContext* ctx, JObject obj, FieldRef field);
    Float GetFloatField(VMContext* ctx, JObject obj, FieldRef field);
    Double GetDoubleField(VMContext* ctx, JObject obj, FieldRef field);
    JObject GetObjectField(VMContext* ctx, JObject obj, FieldRef field);
    Handle GetHandleField(VMContext* ctx, JObject obj, FieldRef field);

    Bool SetBoolField(VMContext* ctx, JObject obj, FieldRef field, Bool new_value);
    Byte SetByteField(VMContext* ctx, JObject obj, FieldRef field, Byte new_value);
    Short SetShortField(VMContext* ctx, JObject obj, FieldRef field, Short new_value);
    Int SetIntField(VMContext* ctx, JObject obj, FieldRef field, Int new_value);
    Long SetLongField(VMContext* ctx, JObject obj, FieldRef field, Long new_value);
    Char SetCharField(VMContext* ctx, JObject obj, FieldRef field, Char new_value);
    Float SetFloatField(VMContext* ctx, JObject obj, FieldRef field, Float new_value);
    Double SetDoubleField(VMContext* ctx, JObject obj, FieldRef field, Double new_value);
    JObject SetObjectField(VMContext* ctx, JObject obj, FieldRef field, JObject new_value);
    Handle SetHandleField(VMContext* ctx, JObject obj, FieldRef field, Handle new_value);

    // Method operations

    MethodRef GetMethod(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor);
    FunctionRef DispatchMethod(VMContext* ctx, ClassRef clas, MethodRef method);

    void CallVoidMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    void CallVoidMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    void CallVoidMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Bool CallBoolMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Bool CallBoolMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Bool CallBoolMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Byte CallByteMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Byte CallByteMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Byte CallByteMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Short CallShortMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Short CallShortMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Short CallShortMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Int CallIntMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Int CallIntMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Int CallIntMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Long CallLongMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Long CallLongMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Long CallLongMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Char CallCharMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Char CallCharMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Char CallCharMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Float CallFloatMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Float CallFloatMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Float CallFloatMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Double CallDoubleMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Double CallDoubleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Double CallDoubleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    JObject CallObjectMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    JObject CallObjectMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    JObject CallObjectMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Handle CallHandleMethod(VMContext* ctx, MethodRef method, JObject obj, ...);
    Handle CallHandleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Handle CallHandleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    // Primitives::String class operations

    String NewString(VMContext* ctx, const Char* chars, Long length);
    Long GetStringLength(VMContext* ctx, String string);
    char* GetStringChars(VMContext* ctx, String string);
    void ReleaseStringChars(VMContext* ctx, char* chars);

    // Array operations

    Long GetArrayLength(VMContext* ctx, ::Array array);

    ObjectArray NewObjectArray(VMContext* ctx, Long length, ClassRef element_class, JObject initial_element);
    JObject GetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index);
    JObject SetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index, JObject value);

    BoolArray NewBoolArray(VMContext* ctx, Long length);
    ByteArray NewByteArray(VMContext* ctx, Long length);
    ShortArray NewShortArray(VMContext* ctx, Long length);
    IntArray NewIntArray(VMContext* ctx, Long length);
    LongArray NewLongArray(VMContext* ctx, Long length);
    CharArray NewCharArray(VMContext* ctx, Long length);
    FloatArray NewFloatArray(VMContext* ctx, Long length);
    DoubleArray NewDoubleArray(VMContext* ctx, Long length);
    HandleArray NewHandleArray(VMContext* ctx, Long length);

    Bool* GetBoolArrayElements(VMContext* ctx, BoolArray array);
    Byte* GetByteArrayElements(VMContext* ctx, ByteArray array);
    Short* GetShortArrayElements(VMContext* ctx, ShortArray array);
    Int* GetIntArrayElements(VMContext* ctx, IntArray array);
    Long* GetLongArrayElements(VMContext* ctx, LongArray array);
    Char* GetCharArrayElements(VMContext* ctx, CharArray array);
    Float* GetFloatArrayElements(VMContext* ctx, FloatArray array);
    Double* GetDoubleArrayElements(VMContext* ctx, DoubleArray array);
    Handle* GetHandleArrayElements(VMContext* ctx, HandleArray array);

    void ReleaseBoolArrayElements(VMContext* ctx, BoolArray array, Bool* elements, Int mode);
    void ReleaseByteArrayElements(VMContext* ctx, ByteArray array, Byte* elements, Int mode);
    void ReleaseShortArrayElements(VMContext* ctx, ShortArray array, Short* elements, Int mode);
    void ReleaseIntArrayElements(VMContext* ctx, IntArray array, Int* elements, Int mode);
    void ReleaseLongArrayElements(VMContext* ctx, LongArray array, Long* elements, Int mode);
    void ReleaseCharArrayElements(VMContext* ctx, CharArray array, Char* elements, Int mode);
    void ReleaseFloatArrayElements(VMContext* ctx, FloatArray array, Float* elements, Int mode);
    void ReleaseDoubleArrayElements(VMContext* ctx, DoubleArray array, Double* elements, Int mode);
    void ReleaseHandleArrayElements(VMContext* ctx, HandleArray array, Handle* elements, Int mode);
}

#endif // JESUSVM_NATIVE_PROVIDED_FUNCTIONS_H
