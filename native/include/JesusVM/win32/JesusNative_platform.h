#ifndef JESUS_VM_NATIVE_PLATFORM_H
#define JESUS_VM_NATIVE_PLATFORM_H

#define NATIVEEXPORT __declspec(dllexport)
#define NATIVEIMPORT __declspec(dllimport)
#define NATIVECALL __stdcall

typedef signed char		Byte;
typedef long			Int;
typedef long long		Long;

#endif // JESUS_VM_NATIVE_PLATFORM_H