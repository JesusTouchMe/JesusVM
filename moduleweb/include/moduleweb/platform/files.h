// Copyright 2025 JesusTouchMe

#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_PLATFORM_FILES_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_PLATFORM_FILES_H 1

#include "moduleweb/types.h"

#ifdef PLATFORM_WINDOWS
#define WIN32_LEAN_AND_MEAN
#define VC_EXTRALEAN
#include <windows.h>
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PLATFORM_WINDOWS
typedef struct moduleweb_file_info* moduleweb_file;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
typedef int moduleweb_file;
#endif

int moduleweb_file_open_read(moduleweb_file* file, const char* filename);
int moduleweb_file_open_read_utf8(moduleweb_file* file, const char* filename);
int moduleweb_file_open_write(moduleweb_file* file, const char* filename);
int moduleweb_file_open_write_utf8(moduleweb_file* file, const char* filename);
int moduleweb_file_open_read_write(moduleweb_file* file, const char* filename);
int moduleweb_file_open_read_write_utf8(moduleweb_file* file, const char* filename);
int moduleweb_file_close(moduleweb_file* file);

int moduleweb_file_lock(moduleweb_file file);
int moduleweb_file_unlock(moduleweb_file file);

int moduleweb_file_get_size(moduleweb_file file, u64* res);
int moduleweb_file_get_position(moduleweb_file file, u64* res);
int moduleweb_file_is_readable(moduleweb_file file, bool* res);
int moduleweb_file_is_writeable(moduleweb_file file, bool* res);

int moduleweb_file_read(moduleweb_file* file, void* buffer, u64 size, u64* bytes_read);
int moduleweb_file_write(moduleweb_file* file, const void* buffer, u64 size, u64* bytes_written);
int moduleweb_file_seek(moduleweb_file* file, u64 pos);

#ifdef __cplusplus
}
#endif

#endif // JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_PLATFORM_FILES_H
