#ifndef JESUS_VM_NATIVE_PLATFORM_H
#define JESUS_VM_NATIVE_PLATFORM_H

#ifdef PLATFORM_WINDOWS
#define JESUSVM_EXPORT __declspec(dllexport)
#define JESUSVM_IMPORT __declspec(dllimport)
#define JESUSVM_CALL __stdcall
#else
#define JESUSVM_EXPORT
#define JESUSVM_IMPORT
#define JESUSVM_CALL
#endif

typedef signed char		Byte;
typedef long			Int;
typedef long long		Long;

#endif // JESUS_VM_NATIVE_PLATFORM_H