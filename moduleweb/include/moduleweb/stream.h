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
#define MODULEWEB_OUTSTREAM_BUFFER_SIZE (16 * 1024)

typedef enum moduleweb_streaming_mode {
    MODULEWEB_STREAMING_MEMORY,
    MODULEWEB_STREAMING_FILE
} moduleweb_streaming_mode;

typedef struct moduleweb_instream {
    moduleweb_streaming_mode streaming_mode;

    union {
        struct {
            OWNED_OBJECT char* name;
            moduleweb_file file;

            u64 size;
            u64 pos;
        } file;

        struct {
            UNOWNED_OBJECT const u8* ptr;
            u64 size;
            u64 pos;
        } memory;
    };

    OWNED_OBJECT u8* buffer;
    u32 buffer_index;
    u32 buffer_size;

    moduleweb_errno moduleweb_errno;
    i32 sys_errno;
} moduleweb_instream;

typedef struct moduleweb_outstream {
    moduleweb_streaming_mode streaming_mode;

    union {
        struct {
            OWNED_OBJECT char* name;
            moduleweb_file file;
        } file;

        struct {
            UNOWNED_OBJECT u8* ptr;
            u64 size;
            u64 pos;
        } memory;
    };

    OWNED_OBJECT u8* buffer;
    u32 buffer_index;

    moduleweb_errno moduleweb_errno;
    i32 sys_errno;
} moduleweb_outstream;

//
// IN STREAM
//

int moduleweb_instream_open(moduleweb_instream* stream, PARAM_COPIED const char* filename);
int moduleweb_instream_open_utf8(moduleweb_instream* stream, PARAM_COPIED const char* filename);

void moduleweb_instream_close(moduleweb_instream* stream);

int moduleweb_instream_open_buffer(moduleweb_instream* stream, IF(own_buffer, PARAM_MOVED, PARAM_REFERENCED) const u8* buffer, u64 size, bool own_buffer);

void moduleweb_instream_close_buffer(moduleweb_instream* stream);

const char* moduleweb_instream_strerror(moduleweb_instream* stream);

bool moduleweb_instream_is_eof(moduleweb_instream* stream);

int moduleweb_instream_read_bytes(moduleweb_instream* stream, PARAM_MUTATED u8* res, u64 count);

// 1, 2, 4, 8
static inline int moduleweb_instream_read_number(moduleweb_instream* stream, PARAM_MUTATED void* res, u64 size) {
    if (size > 8) return 1;

    u8 raw[8] = {0}; // it should only ever be 8 max
    if (moduleweb_instream_read_bytes(stream, raw, size)) {
        return 1;
    }

    // big endian
    u64 result = 0;
    for (u64 i = 0; i < size; i++) {
        result |= ((u64) raw[i]) << ((size - 1 - i) * 8);
    }

    memcpy_s(res, size, &result, size);

    return 0;
}

static inline int moduleweb_instream_read_u8(moduleweb_instream* stream, PARAM_MUTATED u8* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u8));
}

static inline int moduleweb_instream_read_u16(moduleweb_instream* stream, PARAM_MUTATED u16* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u16));
}

static inline int moduleweb_instream_read_u32(moduleweb_instream* stream, PARAM_MUTATED u32* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u32));
}

static inline int moduleweb_instream_read_u64(moduleweb_instream* stream, PARAM_MUTATED u64* res) {
    return moduleweb_instream_read_number(stream, res, sizeof(u64));
}

u8* moduleweb_instream_pointer(moduleweb_instream* stream);

int moduleweb_instream_skip(moduleweb_instream* stream, u64 amount);

//
// OUT STREAM
//

int moduleweb_outstream_init(moduleweb_outstream* stream, PARAM_COPIED const char* filename);

void moduleweb_outstream_uninit(moduleweb_outstream* stream);

int moduleweb_outstream_init_buffer(moduleweb_outstream* stream, PARAM_MUTATED u8* buffer, u64 size);

int moduleweb_outstream_reopen_buffer(moduleweb_outstream* stream, PARAM_MUTATED u8* new_buffer, u64 size);

void moduleweb_outstream_uninit_buffer(moduleweb_outstream* stream);

const char* moduleweb_outstream_strerror(moduleweb_outstream* stream);

int moduleweb_outstream_write_bytes(moduleweb_outstream* stream, PARAM_COPIED const u8* data, u64 size);

// 1, 2, 4, 8
static inline int moduleweb_outstream_write_number(moduleweb_outstream* stream, const void* value, u64 size) {
    if (size > 8) return 1;

    u64 input = 0;
    memcpy_s(&input, sizeof(u64), value, size);

    u8 raw[8];
    for (u64 i = 0; i < size; i++) {
        raw[i] = (u8) ((input >> ((size - 1 - i) * 8)) & 0xFF);
    }

    return moduleweb_outstream_write_bytes(stream, raw, size);
}

static inline int moduleweb_outstream_write_u8(moduleweb_outstream* stream, u8 value) {
    return moduleweb_outstream_write_number(stream, &value, sizeof(u8));
}

static inline int moduleweb_outstream_write_u16(moduleweb_outstream* stream, u16 value) {
    return moduleweb_outstream_write_number(stream, &value, sizeof(u16));
}

static inline int moduleweb_outstream_write_u32(moduleweb_outstream* stream, u32 value) {
    return moduleweb_outstream_write_number(stream, &value, sizeof(u32));
}

static inline int moduleweb_outstream_write_u64(moduleweb_outstream* stream, u64 value) {
    return moduleweb_outstream_write_number(stream, &value, sizeof(u64));
}

int moduleweb_outstream_flush(moduleweb_outstream* stream);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_STREAM_H
