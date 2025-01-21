#ifndef JESUSVM_MODULEWEB_STREAM_H
#define JESUSVM_MODULEWEB_STREAM_H

#include "moduleweb/error.h"
#include "moduleweb/types.h"

#include "moduleweb/platform/files.h"

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MODULEWEB_INSTREAM_BUFFER_SIZE (16 * 1024)

typedef enum moduleweb_endianness {
    MODULEWEB_LITTLE_ENDIAN,
    MODULEWEB_BIG_ENDIAN,
} moduleweb_endianness;

typedef struct moduleweb_instream {
    char* filename;
    moduleweb_file file;

    u64 file_size;
    u64 file_pos;

    moduleweb_endianness endianness;

    u8 memory[MODULEWEB_INSTREAM_BUFFER_SIZE];
    u32 memory_index;
    u32 memory_size;

    moduleweb_errno moduleweb_errno;
    i32 sys_errno;
} moduleweb_instream;

int moduleweb_instream_open(moduleweb_instream* stream, const char* filename, moduleweb_endianness endianness);

void moduleweb_instream_close(moduleweb_instream* stream);

const char* moduleweb_instream_strerror(moduleweb_instream* stream);

bool moduleweb_instream_is_eof(moduleweb_instream* stream);

int moduleweb_instream_readbytes(moduleweb_instream* stream, u8* res, u64 count);

// 1, 2, 4, 8
static inline int moduleweb_instream_read_number(moduleweb_instream* stream, void* res, u64 size) {
    if (size > 8) return 1;

    u8 raw[8]; // it should only ever be 8 max
    if (moduleweb_instream_readbytes(stream, raw, size)) {
        return 1;
    }

    // big endian
    u64 result = 0;
    for (u64 i = 0; i < size; i++) {
        result |= ((u64) raw[i]) << ((size - 1 - i) * 8);
    }

    memcpy_s(res, size, &result, sizeof(u64));

    return 0;
}

static inline int moduleweb_instream_read_u8(moduleweb_instream* stream, u8* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u8));
}

static inline int moduleweb_instream_read_u16(moduleweb_instream* stream, u16* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u16));
}

static inline int moduleweb_instream_read_u32(moduleweb_instream* stream, u32* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u32));
}

static inline int moduleweb_instream_read_u64(moduleweb_instream* stream, u64* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u64));
}

int moduleweb_instream_skip(moduleweb_instream* stream, u64 amount);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_STREAM_H
