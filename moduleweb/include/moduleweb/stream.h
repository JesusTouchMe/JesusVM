#ifndef JESUSVM_MODULEWEB_STREAM_H
#define JESUSVM_MODULEWEB_STREAM_H

#include "moduleweb/error.h"
#include "moduleweb/types.h"

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_stream {
    char* filename;
    int fd;
    struct stat stat;

    void* memory;
    u32 memory_i;
    u32 memory_n;

    moduleweb_errno moduleweb_errno;
    i32 sys_errno;
} moduleweb_stream;

i32 moduleweb_stream_open(moduleweb_stream* stream, const char* filename);

void moduleweb_stream_close(moduleweb_stream* stream);

const char* moduleweb_stream_strerror(moduleweb_stream* stream);

bool moduleweb_stream_iseof(moduleweb_stream* stream);

static inline bool moduleweb_stream_readu8(moduleweb_stream* stream, u8* res) {
    u8* mem = stream->memory;
    u32 i = stream->memory_i;

    if (i >= stream->memory_n) {
        stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
    }
}

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_STREAM_H
