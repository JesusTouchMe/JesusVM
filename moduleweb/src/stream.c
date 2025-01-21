#include "moduleweb/stream.h"

#include <stdio.h>

int moduleweb_instream_open(moduleweb_instream* stream, const char* filename, moduleweb_endianness endianness) {
    stream->filename = strdup(filename);
    if (stream->filename == NULL) {
        stream->sys_errno = errno;
        stream->moduleweb_errno = MODULEWEB_ERROR_ERRNO;
        goto fail;
    }

    if (moduleweb_file_open_read(&stream->file, filename)) {
        goto fail_filename;
    }

    if (moduleweb_file_lock(stream->file)) {
        goto fail_file;
    }

    if (moduleweb_file_get_size(stream->file, &stream->file_size)) {
        goto fail_file_locked;
    }

    stream->file_pos = 0;

    stream->memory_index = 0;
    stream->memory_size = 0;

    return 0;

    fail_file_locked:
    moduleweb_file_unlock(stream->file);

    fail_file:
    moduleweb_file_close(&stream->file);

    fail_filename:
    free(stream->filename);

    fail:
    return 1;
}

void moduleweb_instream_close(moduleweb_instream* stream) {
    if (moduleweb_file_close(&stream->file)) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }

    free(stream->filename);
}

const char* moduleweb_instream_strerror(moduleweb_instream* stream) {
    if (stream->moduleweb_errno == MODULEWEB_ERROR_ERRNO) {
        return strerror(stream->sys_errno);
    }

    return moduleweb_strerror(stream->moduleweb_errno);
}

bool moduleweb_instream_is_eof(moduleweb_instream* stream) {
    return stream->file_pos == stream->file_size;
}

int moduleweb_instream_read_bytes(moduleweb_instream* stream, u8* res, u64 count) {
    if (stream->file_pos + count >= stream->file_size) {
        stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
        return 1;
    }

    u64 bytes_read = 0;

    while (bytes_read < count) {
        if (stream->memory_index >= stream->memory_size) {
            u64 byte_count;
            if (moduleweb_file_read(&stream->file, stream->memory, MODULEWEB_INSTREAM_BUFFER_SIZE, &byte_count)) {
                stream->moduleweb_errno = MODULEWEB_ERROR_FILE_READ_FAIL;
                return 1;
            }

            if (byte_count == 0) {
                stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
                return 1;
            }

            stream->memory_index = 0;
            stream->memory_size = byte_count;

            stream->file_pos += byte_count;
        }

        u64 to_copy = count - bytes_read;
        u64 available = stream->memory_size - stream->memory_index;
        if (to_copy > available) to_copy = available;

        memcpy(res + bytes_read, stream->memory + stream->memory_index, to_copy);
        bytes_read += to_copy;
        stream->memory_index += to_copy;
    }

    return 0;
}

int moduleweb_instream_skip(moduleweb_instream* stream, u64 amount) {
    if (stream->file_pos + amount > stream->file_size) {
        stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
        return 1;
    }

    u64 buffer_remaining = stream->memory_size - stream->memory_index;
    if (amount <= buffer_remaining) {
        stream->memory_index += amount;
        stream->file_pos += amount;

        if (moduleweb_file_seek(&stream->file, stream->file_pos)) {
            stream->moduleweb_errno = MODULEWEB_ERROR_FILE_READ_FAIL;
            return 1;
        }

        return 0;
    }

    amount -= buffer_remaining;
    stream->file_pos += buffer_remaining;
    stream->memory_index = stream->memory_size;

    if (moduleweb_file_seek(&stream->file, stream->file_pos + amount)) {
        stream->moduleweb_errno = MODULEWEB_ERROR_FILE_SEEK_FAIL;
        return 1;
    }

    stream->file_pos += amount;
    stream->memory_index = stream->memory_size = 0; // reset buffer

    return 0;
}