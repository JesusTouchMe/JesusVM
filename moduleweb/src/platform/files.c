// Copyright 2025 JesusTouchMe

#include "moduleweb/platform/files.h"

#include <stdbool.h>

#ifdef PLATFORM_WINDOWS
typedef struct moduleweb_file_info {
    HANDLE handle;
    bool is_readable;
    bool is_writeable;
} moduleweb_file_info;
#endif

int moduleweb_file_open_read(moduleweb_file* file, const char* filename) {
    if (file == NULL) return 1;

#ifdef PLATFORM_WINDOWS
    HANDLE handle = CreateFileA(
            filename,
            GENERIC_READ,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        return 1;
    }

    moduleweb_file_info* file_info = malloc(sizeof(moduleweb_file_info));
    if (file_info == NULL) {
        CloseHandle(handle);
        return 1;
    }

    file_info->handle = handle;
    file_info->is_readable = true;
    file_info->is_writeable = false;

    *file = file_info;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    int fd = open(filename, O_RDONLY, 0644);

    if (fd < 0) {
        return 1;
    }

    *file = fd;
#endif

    return 0;
}

int moduleweb_file_open_write(moduleweb_file* file, const char* filename) {
    if (file == NULL) return 1;

#ifdef PLATFORM_WINDOWS
    HANDLE handle = CreateFileA(
            filename,
            GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        return 1;
    }

    moduleweb_file_info* file_info = malloc(sizeof(moduleweb_file_info));
    if (file_info == NULL) {
        CloseHandle(handle);
        return 1;
    }

    file_info->handle = handle;
    file_info->is_readable = false;
    file_info->is_writeable = true;

    *file = file_info;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    int fd = open(filename, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return 1;
    }

    *file = fd;
#endif

    return 0;
}

int moduleweb_file_open_read_write(moduleweb_file* file, const char* filename) {
    if (file == NULL) return 1;

#ifdef PLATFORM_WINDOWS
    HANDLE handle = CreateFileA(
            filename,
            GENERIC_READ | GENERIC_WRITE,
            FILE_SHARE_READ | FILE_SHARE_WRITE,
            NULL,
            OPEN_ALWAYS,
            FILE_ATTRIBUTE_NORMAL,
            NULL
    );

    if (handle == INVALID_HANDLE_VALUE) {
        return 1;
    }

    moduleweb_file_info* file_info = malloc(sizeof(moduleweb_file_info));
    if (file_info == NULL) {
        CloseHandle(handle);
        return 1;
    }

    file_info->handle = handle;
    file_info->is_readable = true;
    file_info->is_writeable = true;

    *file = file_info;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    int fd = open(filename, O_RDWR | O_CREAT, 0644);

    if (fd < 0) {
        return 1;
    }

    *file = fd;
#endif

    return 0;
}

int moduleweb_file_close(moduleweb_file* file) {
    if (file == NULL) return 1;

#ifdef PLATFORM_WINDOWS
    moduleweb_file_info* info = *file;
    CloseHandle(info->handle);
    free(info);
    *file = NULL;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    close(*file);
    *file = -1;
#endif

    return 0;
}

int moduleweb_file_lock(moduleweb_file file) {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file->handle, &size)) {
        return 1;
    }

    if (!LockFile(file->handle, 0, 0, size.LowPart, size.HighPart)) {
        return 1;
    }
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    if (flock(file, LOCK_EX) < 0) {
        return 1;
    }
#endif

    return 0;
}

int moduleweb_file_unlock(moduleweb_file file) {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file->handle, &size)) {
        return 1;
    }

    if (!UnlockFile(file->handle, 0, 0, size.LowPart, size.HighPart)) {
        return 1;
    }
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    if (flock(file, LOCK_UN) < 0) {
        return 1;
    }
#endif

    return 0;
}

int moduleweb_file_get_size(moduleweb_file file, u64* res) {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER size;
    if (!GetFileSizeEx(file->handle, &size)) {
        return 1;
    }

    *res = size.QuadPart;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    struct stat st;
    if (fstat(file, &st) < 0) {
        return 1;
    }

    *res = st.st_size;
#endif

    return 0;
}

int moduleweb_file_get_position(moduleweb_file file, u64* res) {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER zero = {0};
    LARGE_INTEGER position = {0};

    if (!SetFilePointerEx(file->handle, zero, &position, FILE_CURRENT)) {
        return 1;
    }

    *res = position.QuadPart;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    off_t position = lseek(file, 0, SEEK_CUR);
    *res = (position < 0) ? 0 : (u64) position;
#endif

    return 0;
}

int moduleweb_file_is_readable(moduleweb_file file, bool* res) {
#ifdef PLATFORM_WINDOWS
    *res = file->is_readable;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    int access = fcntl(file, F_GETFL);
    *res = (access & O_RDONLY) || (access & O_RDWR);
#endif

    return 0;
}

int moduleweb_file_is_writeable(moduleweb_file file, bool* res) {
#ifdef PLATFORM_WINDOWS
    *res = file->is_writeable;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    int access = fcntl(file, F_GETFL);
    *res = (access & O_WRONLY) || (access & O_RDWR);
#endif

    return 0;
}

int moduleweb_file_read(moduleweb_file* file, void* buffer, u64 size, u64* bytes_read) {
    bool is_readable;
    if (!moduleweb_file_is_readable(*file, &is_readable) && !is_readable) {
        return 1;
    }

#ifdef PLATFORM_WINDOWS
    DWORD dwBytesRead = 0;
    moduleweb_file_info* info = (moduleweb_file_info*) *file;

    if (!ReadFile(info->handle, buffer, (DWORD) size, &dwBytesRead, NULL)) {
        return 1;
    }

    if (bytes_read != NULL) *bytes_read = (u64) dwBytesRead;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    ssize_t _bytes_read = read(*file, buffer, size);
    if (bytes_read != NULL) *bytes_read = (u64) (_bytes_read < 0) ? 0 : _bytes_read;
#endif
    return 0;
}

int moduleweb_file_write(moduleweb_file* file, const void* buffer, u64 size, u64* bytes_written) {
    bool is_writeable;
    if (!moduleweb_file_is_writeable(*file, &is_writeable) && !is_writeable) {
        return 1;
    }

#ifdef PLATFORM_WINDOWS
    DWORD dwBytesWritten = 0;
    moduleweb_file_info* info = *file;

    if (!WriteFile(info->handle, buffer, (DWORD) size, &dwBytesWritten, NULL)) {
        return 1;
    }

    if (bytes_written != NULL) *bytes_written = (u64) dwBytesWritten;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    ssize_t _bytes_written = write(*file, buffer, size);
    if (bytes_written != NULL) *bytes_written = (u64) (_bytes_written < 0) ? 0 : _bytes_written;
#endif
    return 0;
}

int moduleweb_file_seek(moduleweb_file* file, u64 pos) {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER move;
    move.QuadPart = (LONGLONG) pos;

    moduleweb_file_info* info = *file;

    if (!SetFilePointerEx(info->handle, move, NULL, FILE_BEGIN)) {
        return 1;
    }
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    off_t result = lseek(*file, (off_t) pos, SEEK_SET);
    return (result < 0) ? 1 : 0;
#endif

    return 0;
}
