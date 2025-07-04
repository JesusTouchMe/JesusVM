#ifndef JESUS_VM_NATIVE_PLATFORM_H
#define JESUS_VM_NATIVE_PLATFORM_H

// TODO: implement for all platforms
#define JESUSVM_NORETURN __attribute__((noreturn))

#ifdef PLATFORM_WINDOWS
#define JESUSVM_EXPORT __declspec(dllexport)
#define JESUSVM_IMPORT __declspec(dllimport)
#else
#define JESUSVM_EXPORT
#define JESUSVM_IMPORT
#endif

#endif // JESUS_VM_NATIVE_PLATFORM_H