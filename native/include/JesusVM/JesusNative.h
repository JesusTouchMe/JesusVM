#ifndef JESUS_VM_NATIVE_H
#define JESUS_VM_NATIVE_H

#ifdef __cplusplus
#include <cstddef>
#include <type_traits>
#endif

#include <stdarg.h>
#include <stdint.h>

#include "JesusNative_platform.h"

typedef uint8_t	        Bool;
typedef int8_t  		Byte;
typedef int16_t         Short;
typedef int32_t         Int;
typedef int64_t         Long;
typedef char            Char;
typedef float			Float;
typedef double			Double;
typedef void*           Handle;

typedef Int 			VMErr;

#define VM_OK			    0

#define VM_COMMIT           1
#define VM_ABORT            2

#define VM_RETAIN_MODULE    1
#define VM_LINK_GLOBAL      2

#undef GetClassName

#ifdef __cplusplus

class _Object {};
class _String : public _Object {};
class _Array : public _Object {};
class _BoolArray : public _Array {};
class _ByteArray : public _Array {};
class _ShortArray : public _Array {};
class _IntArray : public _Array {};
class _LongArray : public _Array {};
class _CharArray : public _Array {};
class _FloatArray : public _Array {};
class _DoubleArray : public _Array {};
class _HandleArray : public _Array {};
class _ObjectArray : public _Array {};

typedef _Object* JObject;
typedef _String* String;
typedef _Array* Array;
typedef _BoolArray* BoolArray;
typedef _ByteArray* ByteArray;
typedef _ShortArray* ShortArray;
typedef _IntArray* IntArray;
typedef _LongArray* LongArray;
typedef _CharArray* CharArray;
typedef _FloatArray* FloatArray;
typedef _DoubleArray* DoubleArray;
typedef _HandleArray* HandleArray;
typedef _ObjectArray* ObjectArray;

#else

struct _Object;

typedef struct _Object* JObject;
typedef JObject         JString;
typedef JObject         Array;
typedef Array           BoolArray;
typedef Array           ByteArray;
typedef Array           ShortArray;
typedef Array           IntArray;
typedef Array           LongArray;
typedef Array           CharArray;
typedef Array           FloatArray;
typedef Array           DoubleArray;
typedef Array           HandleArray;
typedef Array           ObjectArray;

#endif

typedef union JValue {
	Bool Z;
	Byte B;
	Short S;
	Int I;
	Long L;
    Char C;
    Float F;
    Double D;
	JObject R;
    Handle H;

#ifdef __cplusplus
    constexpr JValue() : L(0) {}
    constexpr JValue(std::nullptr_t) : H(nullptr) {}

    template<typename T>
    constexpr JValue(T val) {
        if constexpr (std::is_same_v<T, Bool>) {
            Z = val;
        } else if constexpr (std::is_same_v<T, Byte>) {
            B = val;
        } else if constexpr (std::is_same_v<T, Short>) {
            S = val;
        } else if constexpr (std::is_same_v<T, Int>) {
            I = val;
        } else if constexpr (std::is_same_v<T, Long>) {
            L = val;
        } else if constexpr (std::is_same_v<T, Char>) {
            C = val;
        } else if constexpr (std::is_same_v<T, Float>) {
            F = val;
        } else if constexpr (std::is_same_v<T, Double>) {
            D = val;
        } else if constexpr (std::is_same_v<T, JObject>) {
            R = val;
        } else if constexpr (std::is_same_v<T, Handle>) {
            H = val;
        }
    }
#endif
} JValue;

// Opaque reference types to internal components

typedef struct _ModuleRef* ModuleRef;
typedef struct _GlobalVarRef* GlobalVarRef;
typedef struct _FunctionRef* FunctionRef;
typedef struct _ClassRef* ClassRef;
typedef struct _FieldRef* FieldRef;
typedef struct _MethodRef* MethodRef;

typedef struct _TrapInfo TrapInfo;

typedef struct _ArtificialClass ArtificialClass;
typedef struct _ArtificialField ArtificialField;
typedef struct _ArtificialMethod ArtificialMethod;

struct _JesusVMNativeInterface;
struct _JesusVMContext;

#ifdef __cplusplus
typedef _JesusVMContext VMContext;
#else
typedef _JesusVMNativeInterface* VMContext;
#endif

struct _TrapInfo {
    JObject message;
    struct {
        const char* file;
        Int line;
        Int column;
    } location;

    // DANGEROUS TO SET THIS TO TRUE
    bool recoverable;
};

struct _ArtificialField {
    Short modifier_bits;
    const char* name;
    const char* descriptor;
};

struct _ArtificialMethod {
    Short modifier_bits;
    const char* name;
    const char* descriptor;
    const Byte* code; // if modifier_bits has NATIVE set to true, this should be a function pointer and code_length should be 0
    Int code_length;
};

struct _ArtificialClass {
    const char* name;
    Short modifier_bits;
    ClassRef super_class;

    Short field_count;
    ArtificialField fields;

    Short method_count;
    struct _NameInfo* methods;
};

struct _JesusVMNativeInterface {
    void* _reserved;

    // Module operations

    ModuleRef (*GetModule)(VMContext* ctx, const char* name);
    ModuleRef (*CreateModuleA)(VMContext* ctx, const char* name, const char* module_path, Int mode);
    ModuleRef (*CreateModuleU)(VMContext* ctx, const char* name, const char* module_path, Int mode);
    ModuleRef (*CreateModuleMem)(VMContext* ctx, const char* name, JObject linker, const Byte* data, Long data_length, Int mode);

    // Error operations

    void (*Trap)(VMContext* ctx, const char* message);
    void (*AddTrapHook)(VMContext* ctx, void (*hook)(VMContext* ctx, TrapInfo* info));

    // References

    Handle (*CreateGlobalRef)(VMContext* ctx, JObject obj);
    void (*DeleteGlobalRef)(VMContext* ctx, Handle ref);

    VMErr (*PushLocalFrame)(VMContext* ctx, Int initial_capacity);
    VMErr (*PopLocalFrame)(VMContext* ctx);
    Handle (*CreateLocalRef)(VMContext* ctx, JObject obj);
    void (*DeleteLocalRef)(VMContext* ctx, Handle ref);

    // Global var operations

    GlobalVarRef (*GetGlobalVar)(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor);

    Bool (*GetBoolGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Byte (*GetByteGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Short (*GetShortGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Int (*GetIntGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Long (*GetLongGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Char (*GetCharGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Float (*GetFloatGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Double (*GetDoubleGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    JObject (*GetObjectGlobalVar)(VMContext* ctx, GlobalVarRef global_var);
    Handle (*GetHandleGlobalVar)(VMContext* ctx, GlobalVarRef global_var);

    Bool (*SetBoolGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Bool new_value);
    Byte (*SetByteGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Byte new_value);
    Short (*SetShortGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Short new_value);
    Int (*SetIntGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Int new_value);
    Long (*SetLongGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Long new_value);
    Char (*SetCharGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Char new_value);
    Float (*SetFloatGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Float new_value);
    Double (*SetDoubleGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Double new_value);
    JObject (*SetObjectGlobalVar)(VMContext* ctx, GlobalVarRef global_var, JObject new_value);
    Handle (*SetHandleGlobalVar)(VMContext* ctx, GlobalVarRef global_var, Handle new_value);

    // Function operations

    FunctionRef (*GetFunction)(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor);

    void (*CallVoidFunction)(VMContext* ctx, FunctionRef function, ...);
    void (*CallVoidFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    void (*CallVoidFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Bool (*CallBoolFunction)(VMContext* ctx, FunctionRef function, ...);
    Bool (*CallBoolFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Bool (*CallBoolFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Byte (*CallByteFunction)(VMContext* ctx, FunctionRef function, ...);
    Byte (*CallByteFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Byte (*CallByteFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Short (*CallShortFunction)(VMContext* ctx, FunctionRef function, ...);
    Short (*CallShortFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Short (*CallShortFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Int (*CallIntFunction)(VMContext* ctx, FunctionRef function, ...);
    Int (*CallIntFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Int (*CallIntFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Long (*CallLongFunction)(VMContext* ctx, FunctionRef function, ...);
    Long (*CallLongFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Long (*CallLongFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Char (*CallCharFunction)(VMContext* ctx, FunctionRef function, ...);
    Char (*CallCharFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Char (*CallCharFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Float (*CallFloatFunction)(VMContext* ctx, FunctionRef function, ...);
    Float (*CallFloatFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Float (*CallFloatFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Double (*CallDoubleFunction)(VMContext* ctx, FunctionRef function, ...);
    Double (*CallDoubleFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Double (*CallDoubleFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    JObject (*CallObjectFunction)(VMContext* ctx, FunctionRef function, ...);
    JObject (*CallObjectFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    JObject (*CallObjectFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    Handle (*CallHandleFunction)(VMContext* ctx, FunctionRef function, ...);
    Handle (*CallHandleFunctionA)(VMContext* ctx, FunctionRef function, const JValue* args);
    Handle (*CallHandleFunctionV)(VMContext* ctx, FunctionRef function, va_list args);

    // Class operations

    ClassRef (*GetClass)(VMContext* ctx, ModuleRef module, const char* name);
    ClassRef (*GetSuperClass)(VMContext* ctx, ClassRef clas);
    const char* (*GetClassName)(VMContext* ctx, ClassRef clas);
    JObject (*GetClassLinker)(VMContext* ctx, ClassRef clas);
    Bool (*IsAssignable)(VMContext* ctx, ClassRef from, ClassRef to);

    ClassRef (*CreateArtificialClass)(VMContext* ctx, ModuleRef module, ArtificialClass* clas);

    // Object operations

    JObject (*AllocObject)(VMContext* ctx, ClassRef clas);
    JObject (*NewObject)(VMContext* ctx, ClassRef clas, FunctionRef constructor, ...);
    JObject (*NewObjectA)(VMContext* ctx, ClassRef clas, FunctionRef constructor, const JValue* args);
    JObject (*NewObjectV)(VMContext* ctx, ClassRef clas, FunctionRef constructor, va_list args);
    ClassRef (*GetObjectClass)(VMContext* ctx, JObject obj);
    Bool (*IsInstance)(VMContext* ctx, JObject obj, ClassRef clas);

    // Field operations

    FieldRef (*GetField)(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor);

    Bool (*GetBoolField)(VMContext* ctx, JObject obj, FieldRef field);
    Byte (*GetByteField)(VMContext* ctx, JObject obj, FieldRef field);
    Short (*GetShortField)(VMContext* ctx, JObject obj, FieldRef field);
    Int (*GetIntField)(VMContext* ctx, JObject obj, FieldRef field);
    Long (*GetLongField)(VMContext* ctx, JObject obj, FieldRef field);
    Char (*GetCharField)(VMContext* ctx, JObject obj, FieldRef field);
    Float (*GetFloatField)(VMContext* ctx, JObject obj, FieldRef field);
    Double (*GetDoubleField)(VMContext* ctx, JObject obj, FieldRef field);
    JObject (*GetObjectField)(VMContext* ctx, JObject obj, FieldRef field);
    Handle (*GetHandleField)(VMContext* ctx, JObject obj, FieldRef field);

    Bool (*SetBoolField)(VMContext* ctx, JObject obj, FieldRef field, Bool new_value);
    Byte (*SetByteField)(VMContext* ctx, JObject obj, FieldRef field, Byte new_value);
    Short (*SetShortField)(VMContext* ctx, JObject obj, FieldRef field, Short new_value);
    Int (*SetIntField)(VMContext* ctx, JObject obj, FieldRef field, Int new_value);
    Long (*SetLongField)(VMContext* ctx, JObject obj, FieldRef field, Long new_value);
    Char (*SetCharField)(VMContext* ctx, JObject obj, FieldRef field, Char new_value);
    Float (*SetFloatField)(VMContext* ctx, JObject obj, FieldRef field, Float new_value);
    Double (*SetDoubleField)(VMContext* ctx, JObject obj, FieldRef field, Double new_value);
    JObject (*SetObjectField)(VMContext* ctx, JObject obj, FieldRef field, JObject new_value);
    Handle (*SetHandleField)(VMContext* ctx, JObject obj, FieldRef field, Handle new_value);

    // Method operations

    MethodRef (*GetMethod)(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor);
    FunctionRef (*DispatchMethod)(VMContext* ctx, ClassRef clas, MethodRef method);

    void (*CallVoidMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    void (*CallVoidMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    void (*CallVoidMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Bool (*CallBoolMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Bool (*CallBoolMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Bool (*CallBoolMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Byte (*CallByteMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Byte (*CallByteMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Byte (*CallByteMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Short (*CallShortMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Short (*CallShortMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Short (*CallShortMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Int (*CallIntMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Int (*CallIntMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Int (*CallIntMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Long (*CallLongMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Long (*CallLongMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Long (*CallLongMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Char (*CallCharMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Char (*CallCharMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Char (*CallCharMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Float (*CallFloatMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Float (*CallFloatMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Float (*CallFloatMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Double (*CallDoubleMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Double (*CallDoubleMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Double (*CallDoubleMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    JObject (*CallObjectMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    JObject (*CallObjectMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    JObject (*CallObjectMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    Handle (*CallHandleMethod)(VMContext* ctx, MethodRef method, JObject obj, ...);
    Handle (*CallHandleMethodA)(VMContext* ctx, MethodRef method, JObject obj, const JValue* args);
    Handle (*CallHandleMethodV)(VMContext* ctx, MethodRef method, JObject obj, va_list args);

    // Primitives::String class operations

    String (*NewString)(VMContext* ctx, const Char* chars, Long length);
    Long (*GetStringLength)(VMContext* ctx, String string);
    char* (*GetStringChars)(VMContext* ctx, String string);
    void (*ReleaseStringChars)(VMContext* ctx, char* chars);

    // Array operations

    Long (*GetArrayLength)(VMContext* ctx, Array array);

    ObjectArray (*NewObjectArray)(VMContext* ctx, Long length, ClassRef element_class, JObject initial_element);
    JObject (*GetObjectArrayElement)(VMContext* ctx, ObjectArray array, Long index);
    JObject (*SetObjectArrayElement)(VMContext* ctx, ObjectArray array, Long index, JObject value);

    BoolArray (*NewBoolArray)(VMContext* ctx, Long length);
    ByteArray (*NewByteArray)(VMContext* ctx, Long length);
    ShortArray (*NewShortArray)(VMContext* ctx, Long length);
    IntArray (*NewIntArray)(VMContext* ctx, Long length);
    LongArray (*NewLongArray)(VMContext* ctx, Long length);
    CharArray (*NewCharArray)(VMContext* ctx, Long length);
    FloatArray (*NewFloatArray)(VMContext* ctx, Long length);
    DoubleArray (*NewDoubleArray)(VMContext* ctx, Long length);
    HandleArray (*NewHandleArray)(VMContext* ctx, Long length);

    Bool* (*GetBoolArrayElements)(VMContext* ctx, BoolArray array);
    Byte* (*GetByteArrayElements)(VMContext* ctx, ByteArray array);
    Short* (*GetShortArrayElements)(VMContext* ctx, ShortArray array);
    Int* (*GetIntArrayElements)(VMContext* ctx, IntArray array);
    Long* (*GetLongArrayElements)(VMContext* ctx, LongArray array);
    Char* (*GetCharArrayElements)(VMContext* ctx, CharArray array);
    Float* (*GetFloatArrayElements)(VMContext* ctx, FloatArray array);
    Double* (*GetDoubleArrayElements)(VMContext* ctx, DoubleArray array);
    Handle* (*GetHandleArrayElements)(VMContext* ctx, HandleArray array);

    void (*ReleaseBoolArrayElements)(VMContext* ctx, BoolArray array, Bool* elements, Int mode);
    void (*ReleaseByteArrayElements)(VMContext* ctx, ByteArray array, Byte* elements, Int mode);
    void (*ReleaseShortArrayElements)(VMContext* ctx, ShortArray array, Short* elements, Int mode);
    void (*ReleaseIntArrayElements)(VMContext* ctx, IntArray array, Int* elements, Int mode);
    void (*ReleaseLongArrayElements)(VMContext* ctx, LongArray array, Long* elements, Int mode);
    void (*ReleaseCharArrayElements)(VMContext* ctx, CharArray array, Char* elements, Int mode);
    void (*ReleaseFloatArrayElements)(VMContext* ctx, FloatArray array, Float* elements, Int mode);
    void (*ReleaseDoubleArrayElements)(VMContext* ctx, DoubleArray array, Double* elements, Int mode);
    void (*ReleaseHandleArrayElements)(VMContext* ctx, HandleArray array, Handle* elements, Int mode);
};

#ifdef __cplusplus
struct _JesusVMContext {
    _JesusVMNativeInterface* _functions;

   // Module operations

    ModuleRef GetModule(const char* name) {
        return _functions->GetModule(this, name);
    }

    ModuleRef CreateModuleA(const char* name, const char* module_path, Int mode) {
        return _functions->CreateModuleA(this, name, module_path, mode);
    }

    ModuleRef CreateModuleU(const char* name, const char* module_path, Int mode) {
        return _functions->CreateModuleU(this, name, module_path, mode);
    }

    ModuleRef CreateModuleMem(const char* name, JObject linker, const Byte* data, Long data_length, Int mode) {
        return _functions->CreateModuleMem(this, name, linker, data, data_length, mode);
    }

    // Error operations

    void Trap(const char* message) {
        _functions->Trap(this, message);
    }

    void AddTrapHook(void (*hook)(VMContext* ctx, TrapInfo* info)) {
        _functions->AddTrapHook(this, hook);
    }

    // References

    Handle CreateGlobalRef(JObject obj) {
        return _functions->CreateGlobalRef(this, obj);
    }

    void DeleteGlobalRef(Handle ref) {
        _functions->DeleteGlobalRef(this, ref);
    }

    VMErr PushLocalFrame(Int initial_capacity) {
        return _functions->PushLocalFrame(this, initial_capacity);
    }

    VMErr PopLocalFrame(VMContext* ctx) {
        return _functions->PopLocalFrame(this);
    }

    Handle CreateLocalRef(JObject obj) {
        return _functions->CreateLocalRef(this, obj);
    }

    void DeleteLocalRef(Handle ref) {
        _functions->DeleteLocalRef(this, ref);
    }

    // Global var operations

    GlobalVarRef GetGlobalVar(ModuleRef module, const char* name, const char* descriptor) {
        return _functions->GetGlobalVar(this, module, name, descriptor);
    }

    Bool GetBoolGlobalVar(GlobalVarRef global_var) {
        return _functions->GetBoolGlobalVar(this, global_var);
    }

    Byte GetByteGlobalVar(GlobalVarRef global_var) {
        return _functions->GetByteGlobalVar(this, global_var);
    }

    Short GetShortGlobalVar(GlobalVarRef global_var) {
        return _functions->GetShortGlobalVar(this, global_var);
    }

    Int GetIntGlobalVar(GlobalVarRef global_var) {
        return _functions->GetIntGlobalVar(this, global_var);
    }

    Long GetLongGlobalVar(GlobalVarRef global_var) {
        return _functions->GetLongGlobalVar(this, global_var);
    }

    Char GetCharGlobalVar(GlobalVarRef global_var) {
        return _functions->GetCharGlobalVar(this, global_var);
    }

    Float GetFloatGlobalVar(GlobalVarRef global_var) {
        return _functions->GetFloatGlobalVar(this, global_var);
    }

    Double GetDoubleGlobalVar(GlobalVarRef global_var) {
        return _functions->GetDoubleGlobalVar(this, global_var);
    }

    JObject GetObjectGlobalVar(GlobalVarRef global_var) {
        return _functions->GetObjectGlobalVar(this, global_var);
    }

    Handle GetHandleGlobalVar(GlobalVarRef global_var) {
        return _functions->GetHandleGlobalVar(this, global_var);
    }

    Bool SetBoolGlobalVar(GlobalVarRef global_var, bool new_value) {
        return _functions->SetBoolGlobalVar(this, global_var, new_value);
    }

    Byte SetByteGlobalVar(GlobalVarRef global_var, Byte new_value) {
        return _functions->SetByteGlobalVar(this, global_var, new_value);
    }

    Short SetShortGlobalVar(GlobalVarRef global_var, Short new_value) {
        return _functions->SetShortGlobalVar(this, global_var, new_value);
    }

    Int SetIntGlobalVar(GlobalVarRef global_var, Int new_value) {
        return _functions->SetIntGlobalVar(this, global_var, new_value);
    }

    Long SetLongGlobalVar(GlobalVarRef global_var, Long new_value) {
        return _functions->SetLongGlobalVar(this, global_var, new_value);
    }

    Char SetCharGlobalVar(GlobalVarRef global_var, Char new_value) {
        return _functions->SetCharGlobalVar(this, global_var, new_value);
    }

    Float SetFloatGlobalVar(GlobalVarRef global_var, Float new_value) {
        return _functions->SetFloatGlobalVar(this, global_var, new_value);
    }

    Double SetDoubleGlobalVar(GlobalVarRef global_var, Double new_value) {
        return _functions->SetDoubleGlobalVar(this, global_var, new_value);
    }

    JObject SetObjectGlobalVar(GlobalVarRef global_var, JObject new_value) {
        return _functions->SetObjectGlobalVar(this, global_var, new_value);
    }

    Handle SetHandleGlobalVar(GlobalVarRef global_var, Handle new_value) {
        return _functions->SetHandleGlobalVar(this, global_var, new_value);
    }

    // Function operations

    FunctionRef GetFunction(ModuleRef module, const char* name, const char* descriptor) {
        return _functions->GetFunction(this, module, name, descriptor);
    }

    void CallVoidFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        _functions->CallVoidFunctionV(this, function, va);
        va_end(va);
    }

    void CallVoidFunctionA(FunctionRef function, const JValue* args) {
        _functions->CallVoidFunctionA(this, function, args);
    }

    void CallVoidFunctionV(FunctionRef function, va_list args) {
        _functions->CallVoidFunctionV(this, function, args);
    }

    Bool CallBoolFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Bool res = _functions->CallBoolFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Bool CallBoolFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallBoolFunctionA(this, function, args);
    }

    Bool CallBoolFunctionV(FunctionRef function, va_list args) {
        return _functions->CallBoolFunctionV(this, function, args);
    }

    Byte CallByteFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Byte res = _functions->CallByteFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Byte CallByteFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallByteFunctionA(this, function, args);
    }

    Byte CallByteFunctionV(FunctionRef function, va_list args) {
        return _functions->CallByteFunctionV(this, function, args);
    }

    Short CallShortFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Short res = _functions->CallShortFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Short CallShortFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallShortFunctionA(this, function, args);
    }

    Short CallShortFunctionV(FunctionRef function, va_list args) {
        return _functions->CallShortFunctionV(this, function, args);
    }

    Int CallIntFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Int res = _functions->CallIntFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Int CallIntFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallIntFunctionA(this, function, args);
    }

    Int CallIntFunctionV(FunctionRef function, va_list args) {
        return _functions->CallIntFunctionV(this, function, args);
    }

    Long CallLongFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Long res = _functions->CallLongFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Long CallLongFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallLongFunctionA(this, function, args);
    }

    Long CallLongFunctionV(FunctionRef function, va_list args) {
        return _functions->CallLongFunctionV(this, function, args);
    }

    Char CallCharFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Char res = _functions->CallCharFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Char CallCharFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallCharFunctionA(this, function, args);
    }

    Char CallCharFunctionV(FunctionRef function, va_list args) {
        return _functions->CallCharFunctionV(this, function, args);
    }

    Float CallFloatFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Float res = _functions->CallFloatFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Float CallFloatFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallFloatFunctionA(this, function, args);
    }

    Float CallFloatFunctionV(FunctionRef function, va_list args) {
        return _functions->CallFloatFunctionV(this, function, args);
    }

    Double CallDoubleFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Double res = _functions->CallDoubleFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Double CallDoubleFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallDoubleFunctionA(this, function, args);
    }

    Double CallDoubleFunctionV(FunctionRef function, va_list args) {
        return _functions->CallDoubleFunctionV(this, function, args);
    }

    JObject CallObjectFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        JObject res = _functions->CallObjectFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    JObject CallObjectFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallObjectFunctionA(this, function, args);
    }

    JObject CallObjectFunctionV(FunctionRef function, va_list args) {
        return _functions->CallObjectFunctionV(this, function, args);
    }

    Handle CallHandleFunction(FunctionRef function, ...) {
        va_list va;
        va_start(va, function);
        Handle res = _functions->CallHandleFunctionV(this, function, va);
        va_end(va);
        return res;
    }

    Handle CallHandleFunctionA(FunctionRef function, const JValue* args) {
        return _functions->CallHandleFunctionA(this, function, args);
    }

    Handle CallHandleFunctionV(FunctionRef function, va_list args) {
        return _functions->CallHandleFunctionV(this, function, args);
    }

    // Class operations

    ClassRef GetClass(ModuleRef module, const char* name) {
        return _functions->GetClass(this, module, name);
    }

    ClassRef GetSuperClass(ClassRef clas) {
        return _functions->GetSuperClass(this, clas);
    }

    const char* GetClassName(ClassRef clas) {
        return _functions->GetClassName(this, clas);
    }

    JObject GetClassLinker(ClassRef clas) {
        return _functions->GetClassLinker(this, clas);
    }

    Bool IsAssignable(ClassRef from, ClassRef to) {
        return _functions->IsAssignable(this, from, to);
    }

    ClassRef CreateArtificialClass(ModuleRef module, ArtificialClass* clas) {
        return _functions->CreateArtificialClass(this, module, clas);
    }

    // Object operations

    JObject AllocObject(ClassRef clas) {
        return _functions->AllocObject(this, clas);
    }

    JObject NewObject(ClassRef clas, FunctionRef constructor, ...) {
        va_list va;
        va_start(va, constructor);
        JObject res = _functions->NewObject(this, clas, constructor, va);
        va_end(va);
        return res;
    }

    JObject NewObjectA(ClassRef clas, FunctionRef constructor, const JValue* args) {
        return _functions->NewObjectA(this, clas, constructor, args);
    }

    JObject NewObjectV(ClassRef clas, FunctionRef constructor, va_list args) {
        return _functions->NewObjectV(this, clas, constructor, args);
    }

    ClassRef GetObjectClass(JObject obj) {
        return _functions->GetObjectClass(this, obj);
    }

    Bool IsInstance(JObject obj, ClassRef clas) {
        return _functions->IsInstance(this, obj, clas);
    }

    // Field operations

    FieldRef GetField(ClassRef clas, const char* name, const char* descriptor) {
        return _functions->GetField(this, clas, name, descriptor);
    }

    Bool GetBoolField(JObject obj, FieldRef field) {
        return _functions->GetBoolField(this, obj, field);
    }

    Byte GetByteField(JObject obj, FieldRef field) {
        return _functions->GetByteField(this, obj, field);
    }

    Short GetShortField(JObject obj, FieldRef field) {
        return _functions->GetShortField(this, obj, field);
    }

    Int GetIntField(JObject obj, FieldRef field) {
        return _functions->GetIntField(this, obj, field);
    }

    Long GetLongField(JObject obj, FieldRef field) {
        return _functions->GetLongField(this, obj, field);
    }

    Char GetCharField(JObject obj, FieldRef field) {
        return _functions->GetCharField(this, obj, field);
    }

    Float GetFloatField(JObject obj, FieldRef field) {
        return _functions->GetFloatField(this, obj, field);
    }

    Double GetDoubleField(JObject obj, FieldRef field) {
        return _functions->GetDoubleField(this, obj, field);
    }

    JObject GetObjectField(JObject obj, FieldRef field) {
        return _functions->GetObjectField(this, obj, field);
    }

    Handle GetHandleField(JObject obj, FieldRef field) {
        return _functions->GetHandleField(this, obj, field);
    }

    Bool SetBoolField(JObject obj, FieldRef field, Bool new_value) {
        return _functions->SetBoolField(this, obj, field, new_value);
    }

    Byte SetByteField(JObject obj, FieldRef field, Byte new_value) {
        return _functions->SetByteField(this, obj, field, new_value);
    }

    Short SetShortField(JObject obj, FieldRef field, Short new_value) {
        return _functions->SetShortField(this, obj, field, new_value);
    }

    Int SetIntField(JObject obj, FieldRef field, Int new_value) {
        return _functions->SetIntField(this, obj, field, new_value);
    }

    Long SetLongField(JObject obj, FieldRef field, Long new_value) {
        return _functions->SetLongField(this, obj, field, new_value);
    }

    Char SetCharField(JObject obj, FieldRef field, Char new_value) {
        return _functions->SetCharField(this, obj, field, new_value);
    }

    Float SetFloatField(JObject obj, FieldRef field, Float new_value) {
        return _functions->SetFloatField(this, obj, field, new_value);
    }

    Double SetDoubleField(JObject obj, FieldRef field, Double new_value) {
        return _functions->SetDoubleField(this, obj, field, new_value);
    }

    JObject SetObjectField(JObject obj, FieldRef field, JObject new_value) {
        return _functions->SetObjectField(this, obj, field, new_value);
    }

    Handle SetHandleField(JObject obj, FieldRef field, Handle new_value) {
        return _functions->SetHandleField(this, obj, field, new_value);
    }

    // Method operations

    MethodRef GetMethod(ClassRef clas, const char* name, const char* descriptor) {
        return _functions->GetMethod(this, clas, name, descriptor);
    }

    FunctionRef DispatchMethod(ClassRef clas, MethodRef method) {
        return _functions->DispatchMethod(this, clas, method);
    }

    void CallVoidMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        _functions->CallVoidMethodV(this, method, obj, va);
        va_end(va);
    }

    void CallVoidMethodA(MethodRef method, JObject obj, const JValue* args) {
        _functions->CallVoidMethodA(this, method, obj, args);
    }

    void CallVoidMethodV(MethodRef method, JObject obj, va_list args) {
        _functions->CallVoidMethodV(this, method, obj, args);
    }

    Bool CallBoolMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Bool res = _functions->CallBoolMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Bool CallBoolMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallBoolMethodA(this, method, obj, args);
    }

    Bool CallBoolMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallBoolMethodV(this, method, obj, args);
    }

    Byte CallByteMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Byte res = _functions->CallByteMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Byte CallByteMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallByteMethodA(this, method, obj, args);
    }

    Byte CallByteMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallByteMethodV(this, method, obj, args);
    }

    Short CallShortMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Short res = _functions->CallShortMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Short CallShortMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallShortMethodA(this, method, obj, args);
    }

    Short CallShortMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallShortMethodV(this, method, obj, args);
    }

    Int CallIntMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Int res = _functions->CallIntMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Int CallIntMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallIntMethodA(this, method, obj, args);
    }

    Int CallIntMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallIntMethodV(this, method, obj, args);
    }

    Long CallLongMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Long res = _functions->CallLongMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Long CallLongMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallLongMethodA(this, method, obj, args);
    }

    Long CallLongMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallLongMethodV(this, method, obj, args);
    }

    Char CallCharMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Char res = _functions->CallCharMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Char CallCharMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallCharMethodA(this, method, obj, args);
    }

    Char CallCharMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallCharMethodV(this, method, obj, args);
    }

    Float CallFloatMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Float res = _functions->CallFloatMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Float CallFloatMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallFloatMethodA(this, method, obj, args);
    }

    Float CallFloatMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallFloatMethodV(this, method, obj, args);
    }

    Double CallDoubleMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Double res = _functions->CallDoubleMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Double CallDoubleMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallDoubleMethodA(this, method, obj, args);
    }

    Double CallDoubleMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallDoubleMethodV(this, method, obj, args);
    }

    JObject CallObjectMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        JObject res = _functions->CallObjectMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    JObject CallObjectMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallObjectMethodA(this, method, obj, args);
    }

    JObject CallObjectMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallObjectMethodV(this, method, obj, args);
    }

    Handle CallHandleMethod(MethodRef method, JObject obj, ...) {
        va_list va;
        va_start(va, method);
        Handle res = _functions->CallHandleMethodV(this, method, obj, va);
        va_end(va);
        return res;
    }

    Handle CallHandleMethodA(MethodRef method, JObject obj, const JValue* args) {
        return _functions->CallHandleMethodA(this, method, obj, args);
    }

    Handle CallHandleMethodV(MethodRef method, JObject obj, va_list args) {
        return _functions->CallHandleMethodV(this, method, obj, args);
    }

    // Primitives::String class operations

    String NewString(const Char* chars, Long length) {
        return _functions->NewString(this, chars, length);
    }

    Long GetStringLength(String string) {
        return _functions->GetStringLength(this, string);
    }

    char* GetStringChars(String string) {
        return _functions->GetStringChars(this, string);
    }

    void ReleaseStringChars(char* chars) {
        return _functions->ReleaseStringChars(this, chars);
    }

    // Array operations

    Long GetArrayLength(Array array) {
        return _functions->GetArrayLength(this, array);
    }

    ObjectArray NewObjectArray(Long length, ClassRef element_class, JObject initial_element) {
        return _functions->NewObjectArray(this, length, element_class, initial_element);
    }

    JObject GetObjectArrayElement(ObjectArray array, Long index) {
        return _functions->GetObjectArrayElement(this, array, index);
    }

    JObject SetObjectArrayElement(ObjectArray array, Long index, JObject value) {
        return _functions->SetObjectArrayElement(this, array, index, value);
    }

    BoolArray NewBoolArray(Long length) {
        return _functions->NewBoolArray(this, length);
    }

    ByteArray NewByteArray(Long length) {
        return _functions->NewByteArray(this, length);
    }

    ShortArray NewShortArray(Long length) {
        return _functions->NewShortArray(this, length);
    }

    IntArray NewIntArray(Long length) {
        return _functions->NewIntArray(this, length);
    }

    LongArray NewLongArray(Long length) {
        return _functions->NewLongArray(this, length);
    }

    CharArray NewCharArray(Long length) {
        return _functions->NewCharArray(this, length);
    }

    FloatArray NewFloatArray(Long length) {
        return _functions->NewFloatArray(this, length);
    }

    DoubleArray NewDoubleArray(Long length) {
        return _functions->NewDoubleArray(this, length);
    }

    HandleArray NewHandleArray(Long length) {
        return _functions->NewHandleArray(this, length);
    }

    Bool* GetBoolArrayElements(BoolArray array) {
        return _functions->GetBoolArrayElements(this, array);
    }

    Byte* GetByteArrayElements(ByteArray array) {
        return _functions->GetByteArrayElements(this, array);
    }

    Short* GetShortArrayElements(ShortArray array) {
        return _functions->GetShortArrayElements(this, array);
    }

    Int* GetIntArrayElements(IntArray array) {
        return _functions->GetIntArrayElements(this, array);
    }

    Long* GetLongArrayElements(LongArray array) {
        return _functions->GetLongArrayElements(this, array);
    }

    Char* GetCharArrayElements(CharArray array) {
        return _functions->GetCharArrayElements(this, array);
    }

    Float* GetFloatArrayElements(FloatArray array) {
        return _functions->GetFloatArrayElements(this, array);
    }

    Double* GetDoubleArrayElements(DoubleArray array) {
        return _functions->GetDoubleArrayElements(this, array);
    }

    Handle* GetHandleArrayElements(HandleArray array) {
        return _functions->GetHandleArrayElements(this, array);
    }

    void ReleaseBoolArrayElements(BoolArray array, Bool* elements, Int mode) {
        _functions->ReleaseBoolArrayElements(this, array, elements, mode);
    }

    void ReleaseByteArrayElements(ByteArray array, Byte* elements, Int mode) {
        _functions->ReleaseByteArrayElements(this, array, elements, mode);
    }

    void ReleaseShortArrayElements(ShortArray array, Short* elements, Int mode) {
        _functions->ReleaseShortArrayElements(this, array, elements, mode);
    }

    void ReleaseIntArrayElements(IntArray array, Int* elements, Int mode) {
        _functions->ReleaseIntArrayElements(this, array, elements, mode);
    }

    void ReleaseLongArrayElements(LongArray array, Long* elements, Int mode) {
        _functions->ReleaseLongArrayElements(this, array, elements, mode);
    }

    void ReleaseCharArrayElements(CharArray array, Char* elements, Int mode) {
        _functions->ReleaseCharArrayElements(this, array, elements, mode);
    }

    void ReleaseFloatArrayElements(FloatArray array, Float* elements, Int mode) {
        _functions->ReleaseFloatArrayElements(this, array, elements, mode);
    }

    void ReleaseDoubleArrayElements(DoubleArray array, Double* elements, Int mode) {
        _functions->ReleaseDoubleArrayElements(this, array, elements, mode);
    }

    void ReleaseHandleArrayElements(HandleArray array, Handle* elements, Int mode) {
        _functions->ReleaseHandleArrayElements(this, array, elements, mode);
    }
};

#else

inline ModuleRef CreateModuleU(VMContext* ctx, const char* name, const char* module_path, Int mode) {
    return (*ctx)->CreateModuleU(ctx, name, module_path, mode);
}

inline ModuleRef CreateModuleMem(VMContext* ctx, const char* name, JObject linker, const Byte* data, Long data_length, Int mode) {
    return (*ctx)->CreateModuleMem(ctx, name, linker, data, data_length, mode);
}

inline void Trap(VMContext* ctx, const char* message) {
    (*ctx)->Trap(ctx, message);
}

inline void AddTrapHook(VMContext* ctx, void hook(VMContext* ctx, TrapInfo* info)) {
    (*ctx)->AddTrapHook(ctx, hook);
}

inline Handle CreateGlobalRef(VMContext* ctx, JObject obj) {
    return (*ctx)->CreateGlobalRef(ctx, obj);
}

inline void DeleteGlobalRef(VMContext* ctx, Handle ref) {
    (*ctx)->DeleteGlobalRef(ctx, ref);
}

inline VMErr PushLocalFrame(VMContext* ctx, Int initial_capacity) {
    return (*ctx)->PushLocalFrame(ctx, initial_capacity);
}

inline VMErr PopLocalFrame(VMContext* ctx) {
    return (*ctx)->PopLocalFrame(ctx);
}

inline Handle CreateLocalRef(VMContext* ctx, JObject obj) {
    return (*ctx)->CreateLocalRef(ctx, obj);
}

inline void DeleteLocalRef(VMContext* ctx, Handle ref) {
    (*ctx)->DeleteLocalRef(ctx, ref);
}

inline GlobalVarRef GetGlobalVar(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor) {
    return (*ctx)->GetGlobalVar(ctx, module, name, descriptor);
}

inline Bool GetBoolGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetBoolGlobalVar(ctx, global_var);
}

inline Byte GetByteGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetByteGlobalVar(ctx, global_var);
}

inline Short GetShortGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetShortGlobalVar(ctx, global_var);
}

inline Int GetIntGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetIntGlobalVar(ctx, global_var);
}

inline Long GetLongGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetLongGlobalVar(ctx, global_var);
}

inline Char GetCharGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetCharGlobalVar(ctx, global_var);
}

inline Float GetFloatGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetFloatGlobalVar(ctx, global_var);
}

inline Double GetDoubleGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetDoubleGlobalVar(ctx, global_var);
}

inline JObject GetObjectGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetObjectGlobalVar(ctx, global_var);
}

inline Handle GetHandleGlobalVar(VMContext* ctx, GlobalVarRef global_var) {
    return (*ctx)->GetHandleGlobalVar(ctx, global_var);
}

inline Bool SetBoolGlobalVar(VMContext* ctx, GlobalVarRef global_var, Bool new_value) {
    return (*ctx)->SetBoolGlobalVar(ctx, global_var, new_value);
}

inline Byte SetByteGlobalVar(VMContext* ctx, GlobalVarRef global_var, Byte new_value) {
    return (*ctx)->SetByteGlobalVar(ctx, global_var, new_value);
}

inline Short SetShortGlobalVar(VMContext* ctx, GlobalVarRef global_var, Short new_value) {
    return (*ctx)->SetShortGlobalVar(ctx, global_var, new_value);
}

inline Int SetIntGlobalVar(VMContext* ctx, GlobalVarRef global_var, Int new_value) {
    return (*ctx)->SetIntGlobalVar(ctx, global_var, new_value);
}

inline Long SetLongGlobalVar(VMContext* ctx, GlobalVarRef global_var, Long new_value) {
    return (*ctx)->SetLongGlobalVar(ctx, global_var, new_value);
}

inline Char SetCharGlobalVar(VMContext* ctx, GlobalVarRef global_var, Char new_value) {
    return (*ctx)->SetCharGlobalVar(ctx, global_var, new_value);
}

inline Float SetFloatGlobalVar(VMContext* ctx, GlobalVarRef global_var, Float new_value) {
    return (*ctx)->SetFloatGlobalVar(ctx, global_var, new_value);
}

inline Double SetDoubleGlobalVar(VMContext* ctx, GlobalVarRef global_var, Double new_value) {
    return (*ctx)->SetDoubleGlobalVar(ctx, global_var, new_value);
}

inline JObject SetObjectGlobalVar(VMContext* ctx, GlobalVarRef global_var, JObject new_value) {
    return (*ctx)->SetObjectGlobalVar(ctx, global_var, new_value);
}

inline Handle SetHandleGlobalVar(VMContext* ctx, GlobalVarRef global_var, Handle new_value) {
    return (*ctx)->SetHandleGlobalVar(ctx, global_var, new_value);
}

inline FunctionRef GetFunction(VMContext* ctx, ModuleRef module, const char* name, const char* descriptor) {
    return (*ctx)->GetFunction(ctx, module, name, descriptor);
}

inline void CallVoidFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    (*ctx)->CallVoidFunctionV(ctx, function, args);
    va_end(args);
}

inline void CallVoidFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    (*ctx)->CallVoidFunctionA(ctx, function, args);
}

inline void CallVoidFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    (*ctx)->CallVoidFunctionV(ctx, function, args);
}

inline Bool CallBoolFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Bool result = (*ctx)->CallBoolFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Bool CallBoolFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallBoolFunctionA(ctx, function, args);
}

inline Bool CallBoolFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallBoolFunctionV(ctx, function, args);
}

inline Byte CallByteFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Byte result = (*ctx)->CallByteFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Byte CallByteFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallByteFunctionA(ctx, function, args);
}

inline Byte CallByteFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallByteFunctionV(ctx, function, args);
}

inline Short CallShortFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Short result = (*ctx)->CallShortFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Short CallShortFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallShortFunctionA(ctx, function, args);
}

inline Short CallShortFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallShortFunctionV(ctx, function, args);
}

inline Int CallIntFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Int result = (*ctx)->CallIntFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Int CallIntFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallIntFunctionA(ctx, function, args);
}

inline Int CallIntFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallIntFunctionV(ctx, function, args);
}

inline Long CallLongFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Long result = (*ctx)->CallLongFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Long CallLongFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallLongFunctionA(ctx, function, args);
}

inline Long CallLongFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallLongFunctionV(ctx, function, args);
}

inline Char CallCharFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Char result = (*ctx)->CallCharFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Char CallCharFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallCharFunctionA(ctx, function, args);
}

inline Char CallCharFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallCharFunctionV(ctx, function, args);
}

inline Float CallFloatFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Float result = (*ctx)->CallFloatFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Float CallFloatFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallFloatFunctionA(ctx, function, args);
}

inline Float CallFloatFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallFloatFunctionV(ctx, function, args);
}

inline Double CallDoubleFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Double result = (*ctx)->CallDoubleFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Double CallDoubleFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallDoubleFunctionA(ctx, function, args);
}

inline Double CallDoubleFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallDoubleFunctionV(ctx, function, args);
}

inline JObject CallObjectFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    JObject result = (*ctx)->CallObjectFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline JObject CallObjectFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallObjectFunctionA(ctx, function, args);
}

inline JObject CallObjectFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallObjectFunctionV(ctx, function, args);
}

inline Handle CallHandleFunction(VMContext* ctx, FunctionRef function, ...) {
    va_list args;
    va_start(args, function);
    Handle result = (*ctx)->CallHandleFunctionV(ctx, function, args);
    va_end(args);
    return result;
}

inline Handle CallHandleFunctionA(VMContext* ctx, FunctionRef function, const JValue* args) {
    return (*ctx)->CallHandleFunctionA(ctx, function, args);
}

inline Handle CallHandleFunctionV(VMContext* ctx, FunctionRef function, va_list args) {
    return (*ctx)->CallHandleFunctionV(ctx, function, args);
}

inline ClassRef GetClass(VMContext* ctx, ModuleRef module, const char* name) {
    return (*ctx)->GetClass(ctx, module, name);
}

inline ClassRef GetSuperClass(VMContext* ctx, ClassRef clas) {
    return (*ctx)->GetSuperClass(ctx, clas);
}

inline const char* GetClassName(VMContext* ctx, ClassRef clas) {
    return (*ctx)->GetClassName(ctx, clas);
}

inline JObject GetClassLinker(VMContext* ctx, ClassRef clas) {
    return (*ctx)->GetClassLinker(ctx, clas);
}

inline Bool IsAssignable(VMContext* ctx, ClassRef from, ClassRef to) {
    return (*ctx)->IsAssignable(ctx, from, to);
}

inline ClassRef CreateArtificialClass(VMContext* ctx, ModuleRef module, ArtificialClass* clas) {
    return (*ctx)->CreateArtificialClass(ctx, module, clas);
}

inline JObject AllocObject(VMContext* ctx, ClassRef clas) {
    return (*ctx)->AllocObject(ctx, clas);
}

inline JObject NewObject(VMContext* ctx, ClassRef clas, FunctionRef constructor, ...) {
    va_list args;
    va_start(args, constructor);
    JObject result = (*ctx)->NewObjectV(ctx, clas, constructor, args);
    va_end(args);
    return result;
}

inline JObject NewObjectA(VMContext* ctx, ClassRef clas, FunctionRef constructor, const JValue* args) {
    return (*ctx)->NewObjectA(ctx, clas, constructor, args);
}

inline JObject NewObjectV(VMContext* ctx, ClassRef clas, FunctionRef constructor, va_list args) {
    return (*ctx)->NewObjectV(ctx, clas, constructor, args);
}

inline ClassRef GetObjectClass(VMContext* ctx, JObject obj) {
    return (*ctx)->GetObjectClass(ctx, obj);
}

inline Bool IsInstance(VMContext* ctx, JObject obj, ClassRef clas) {
    return (*ctx)->IsInstance(ctx, obj, clas);
}

inline FieldRef GetField(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor) {
    return (*ctx)->GetField(ctx, clas, name, descriptor);
}

inline Bool GetBoolField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetBoolField(ctx, obj, field);
}

inline Byte GetByteField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetByteField(ctx, obj, field);
}

inline Short GetShortField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetShortField(ctx, obj, field);
}

inline Int GetIntField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetIntField(ctx, obj, field);
}

inline Long GetLongField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetLongField(ctx, obj, field);
}

inline Char GetCharField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetCharField(ctx, obj, field);
}

inline Float GetFloatField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetFloatField(ctx, obj, field);
}

inline Double GetDoubleField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetDoubleField(ctx, obj, field);
}

inline JObject GetObjectField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetObjectField(ctx, obj, field);
}

inline Handle GetHandleField(VMContext* ctx, JObject obj, FieldRef field) {
    return (*ctx)->GetHandleField(ctx, obj, field);
}

inline Bool SetBoolField(VMContext* ctx, JObject obj, FieldRef field, Bool new_value) {
    return (*ctx)->SetBoolField(ctx, obj, field, new_value);
}

inline Byte SetByteField(VMContext* ctx, JObject obj, FieldRef field, Byte new_value) {
    return (*ctx)->SetByteField(ctx, obj, field, new_value);
}

inline Short SetShortField(VMContext* ctx, JObject obj, FieldRef field, Short new_value) {
    return (*ctx)->SetShortField(ctx, obj, field, new_value);
}

inline Int SetIntField(VMContext* ctx, JObject obj, FieldRef field, Int new_value) {
    return (*ctx)->SetIntField(ctx, obj, field, new_value);
}

inline Long SetLongField(VMContext* ctx, JObject obj, FieldRef field, Long new_value) {
    return (*ctx)->SetLongField(ctx, obj, field, new_value);
}

inline Char SetCharField(VMContext* ctx, JObject obj, FieldRef field, Char new_value) {
    return (*ctx)->SetCharField(ctx, obj, field, new_value);
}

inline Float SetFloatField(VMContext* ctx, JObject obj, FieldRef field, Float new_value) {
    return (*ctx)->SetFloatField(ctx, obj, field, new_value);
}

inline Double SetDoubleField(VMContext* ctx, JObject obj, FieldRef field, Double new_value) {
    return (*ctx)->SetDoubleField(ctx, obj, field, new_value);
}

inline JObject SetObjectField(VMContext* ctx, JObject obj, FieldRef field, JObject new_value) {
    return (*ctx)->SetObjectField(ctx, obj, field, new_value);
}

inline Handle SetHandleField(VMContext* ctx, JObject obj, FieldRef field, Handle new_value) {
    return (*ctx)->SetHandleField(ctx, obj, field, new_value);
}

inline MethodRef GetMethod(VMContext* ctx, ClassRef clas, const char* name, const char* descriptor) {
    return (*ctx)->GetMethod(ctx, clas, name, descriptor);
}

inline FunctionRef DispatchMethod(VMContext* ctx, ClassRef clas, MethodRef method) {
    return (*ctx)->DispatchMethod(ctx, clas, method);
}

inline void CallVoidMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    (*ctx)->CallVoidMethodV(ctx, method, obj, args);
    va_end(args);
}

inline void CallVoidMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    (*ctx)->CallVoidMethodA(ctx, method, obj, args);
}

inline void CallVoidMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    (*ctx)->CallVoidMethodV(ctx, method, obj, args);
}

inline Bool CallBoolMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Bool result = (*ctx)->CallBoolMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Bool CallBoolMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallBoolMethodA(ctx, method, obj, args);
}

inline Bool CallBoolMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallBoolMethodV(ctx, method, obj, args);
}

inline Byte CallByteMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Byte result = (*ctx)->CallByteMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Byte CallByteMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallByteMethodA(ctx, method, obj, args);
}

inline Byte CallByteMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallByteMethodV(ctx, method, obj, args);
}

inline Short CallShortMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Short result = (*ctx)->CallShortMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Short CallShortMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallShortMethodA(ctx, method, obj, args);
}

inline Short CallShortMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallShortMethodV(ctx, method, obj, args);
}

inline Int CallIntMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Int result = (*ctx)->CallIntMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Int CallIntMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallIntMethodA(ctx, method, obj, args);
}

inline Int CallIntMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallIntMethodV(ctx, method, obj, args);
}

inline Long CallLongMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Long result = (*ctx)->CallLongMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Long CallLongMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallLongMethodA(ctx, method, obj, args);
}

inline Long CallLongMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallLongMethodV(ctx, method, obj, args);
}

inline Char CallCharMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Char result = (*ctx)->CallCharMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Char CallCharMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallCharMethodA(ctx, method, obj, args);
}

inline Char CallCharMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallCharMethodV(ctx, method, obj, args);
}

inline Float CallFloatMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Float result = (*ctx)->CallFloatMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Float CallFloatMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallFloatMethodA(ctx, method, obj, args);
}

inline Float CallFloatMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallFloatMethodV(ctx, method, obj, args);
}

inline Double CallDoubleMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Double result = (*ctx)->CallDoubleMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Double CallDoubleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallDoubleMethodA(ctx, method, obj, args);
}

inline Double CallDoubleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallDoubleMethodV(ctx, method, obj, args);
}

inline JObject CallObjectMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    JObject result = (*ctx)->CallObjectMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline JObject CallObjectMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallObjectMethodA(ctx, method, obj, args);
}

inline JObject CallObjectMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallObjectMethodV(ctx, method, obj, args);
}

inline Handle CallHandleMethod(VMContext* ctx, MethodRef method, JObject obj, ...) {
    va_list args;
    va_start(args, obj);
    Handle result = (*ctx)->CallHandleMethodV(ctx, method, obj, args);
    va_end(args);
    return result;
}

inline Handle CallHandleMethodA(VMContext* ctx, MethodRef method, JObject obj, const JValue* args) {
    return (*ctx)->CallHandleMethodA(ctx, method, obj, args);
}

inline Handle CallHandleMethodV(VMContext* ctx, MethodRef method, JObject obj, va_list args) {
    return (*ctx)->CallHandleMethodV(ctx, method, obj, args);
}

inline String NewString(VMContext* ctx, const Char* chars, Long length) {
    return (*ctx)->NewString(ctx, chars, length);
}

inline Long GetStringLength(VMContext* ctx, String string) {
    return (*ctx)->GetStringLength(ctx, string);
}

inline char* GetStringChars(VMContext* ctx, String string) {
    return (*ctx)->GetStringChars(ctx, string);
}

inline void ReleaseStringChars(VMContext* ctx, char* chars) {
    (*ctx)->ReleaseStringChars(ctx, chars);
}

inline Long GetArrayLength(VMContext* ctx, Array array) {
    return (*ctx)->GetArrayLength(ctx, array);
}

inline ObjectArray NewObjectArray(VMContext* ctx, Long length, ClassRef element_class, JObject initial_element) {
    return (*ctx)->NewObjectArray(ctx, length, element_class, initial_element);
}

inline JObject GetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index) {
    return (*ctx)->GetObjectArrayElement(ctx, array, index);
}

inline JObject SetObjectArrayElement(VMContext* ctx, ObjectArray array, Long index, JObject value) {
    return (*ctx)->SetObjectArrayElement(ctx, array, index, value);
}

inline BoolArray NewBoolArray(VMContext* ctx, Long length) {
    return (*ctx)->NewBoolArray(ctx, length);
}

inline ByteArray NewByteArray(VMContext* ctx, Long length) {
    return (*ctx)->NewByteArray(ctx, length);
}

inline ShortArray NewShortArray(VMContext* ctx, Long length) {
    return (*ctx)->NewShortArray(ctx, length);
}

inline IntArray NewIntArray(VMContext* ctx, Long length) {
    return (*ctx)->NewIntArray(ctx, length);
}

inline LongArray NewLongArray(VMContext* ctx, Long length) {
    return (*ctx)->NewLongArray(ctx, length);
}

inline CharArray NewCharArray(VMContext* ctx, Long length) {
    return (*ctx)->NewCharArray(ctx, length);
}

inline FloatArray NewFloatArray(VMContext* ctx, Long length) {
    return (*ctx)->NewFloatArray(ctx, length);
}

inline DoubleArray NewDoubleArray(VMContext* ctx, Long length) {
    return (*ctx)->NewDoubleArray(ctx, length);
}

inline HandleArray NewHandleArray(VMContext* ctx, Long length) {
    return (*ctx)->NewHandleArray(ctx, length);
}

inline Bool* GetBoolArrayElements(VMContext* ctx, BoolArray array) {
    return (*ctx)->GetBoolArrayElements(ctx, array);
}

inline Byte* GetByteArrayElements(VMContext* ctx, ByteArray array) {
    return (*ctx)->GetByteArrayElements(ctx, array);
}

inline Short* GetShortArrayElements(VMContext* ctx, ShortArray array) {
    return (*ctx)->GetShortArrayElements(ctx, array);
}

inline Int* GetIntArrayElements(VMContext* ctx, IntArray array) {
    return (*ctx)->GetIntArrayElements(ctx, array);
}

inline Long* GetLongArrayElements(VMContext* ctx, LongArray array) {
    return (*ctx)->GetLongArrayElements(ctx, array);
}

inline Char* GetCharArrayElements(VMContext* ctx, CharArray array) {
    return (*ctx)->GetCharArrayElements(ctx, array);
}

inline Float* GetFloatArrayElements(VMContext* ctx, FloatArray array) {
    return (*ctx)->GetFloatArrayElements(ctx, array);
}

inline Double* GetDoubleArrayElements(VMContext* ctx, DoubleArray array) {
    return (*ctx)->GetDoubleArrayElements(ctx, array);
}

inline Handle* GetHandleArrayElements(VMContext* ctx, HandleArray array) {
    return (*ctx)->GetHandleArrayElements(ctx, array);
}

inline void ReleaseBoolArrayElements(VMContext* ctx, BoolArray array, Bool* elements, Int mode) {
    (*ctx)->ReleaseBoolArrayElements(ctx, array, elements, mode);
}

inline void ReleaseByteArrayElements(VMContext* ctx, ByteArray array, Byte* elements, Int mode) {
    (*ctx)->ReleaseByteArrayElements(ctx, array, elements, mode);
}

inline void ReleaseShortArrayElements(VMContext* ctx, ShortArray array, Short* elements, Int mode) {
    (*ctx)->ReleaseShortArrayElements(ctx, array, elements, mode);
}

inline void ReleaseIntArrayElements(VMContext* ctx, IntArray array, Int* elements, Int mode) {
    (*ctx)->ReleaseIntArrayElements(ctx, array, elements, mode);
}

inline void ReleaseLongArrayElements(VMContext* ctx, LongArray array, Long* elements, Int mode) {
    (*ctx)->ReleaseLongArrayElements(ctx, array, elements, mode);
}

inline void ReleaseCharArrayElements(VMContext* ctx, CharArray array, Char* elements, Int mode) {
    (*ctx)->ReleaseCharArrayElements(ctx, array, elements, mode);
}

inline void ReleaseFloatArrayElements(VMContext* ctx, FloatArray array, Float* elements, Int mode) {
    (*ctx)->ReleaseFloatArrayElements(ctx, array, elements, mode);
}

inline void ReleaseDoubleArrayElements(VMContext* ctx, DoubleArray array, Double* elements, Int mode) {
    (*ctx)->ReleaseDoubleArrayElements(ctx, array, elements, mode);
}

inline void ReleaseHandleArrayElements(VMContext* ctx, HandleArray array, Handle* elements, Int mode) {
    (*ctx)->ReleaseHandleArrayElements(ctx, array, elements, mode);
}

#endif

#endif // JESUS_VM_NATIVE_H
