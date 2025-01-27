#ifndef JESUS_VM_NATIVE_PLATFORM_H
#define JESUS_VM_NATIVE_PLATFORM_H

#ifdef PLATFORM_WINDOWS
#define NATIVEEXPORT __declspec(dllexport)
#define NATIVEIMPORT __declspec(dllimport)
#define NATIVECALL __stdcall
#else
#define NATIVEEXPORT
#define NATIVEIMPORT
#define NATIVECALL
#endif

typedef signed char		Byte;
typedef long			Int;
typedef long long		Long;

#endif // JESUS_VM_NATIVE_PLATFORM_H