#include "moduleweb/stream.h"

#include <stdio.h>
#include <errno.h>

#define min(a, b) (((a) < (b)) ? (a) : (b))

int moduleweb_instream_open(moduleweb_instream* stream, const char* filename) {
    stream->file.name = strdup(filename);
    if (stream->file.name == NULL) {
        stream->sys_errno = errno;
        stream->moduleweb_errno = MODULEWEB_ERROR_ERRNO;
        goto fail;
    }

    if (moduleweb_file_open_read(&stream->file.file, filename)) {
        goto fail_filename;
    }

    if (moduleweb_file_lock(stream->file.file)) {
        goto fail_file;
    }

    if (moduleweb_file_get_size(stream->file.file, &stream->file.size)) {
        goto fail_file_locked;
    }

    stream->file.pos = 0;

    stream->buffer = malloc(MODULEWEB_INSTREAM_BUFFER_SIZE);
    stream->buffer_index = 0;
    stream->buffer_size = 0;

    stream->streaming_mode = MODULEWEB_STREAMING_FILE;

    return 0;

    fail_file_locked:
    moduleweb_file_unlock(stream->file.file);

    fail_file:
    moduleweb_file_close(&stream->file.file);

    fail_filename:
    free(stream->file.name);

    fail:
    return 1;
}

int moduleweb_instream_open_utf8(moduleweb_instream* stream, const char* filename) {
    stream->file.name = strdup(filename);
    if (stream->file.name == NULL) {
        stream->sys_errno = errno;
        stream->moduleweb_errno = MODULEWEB_ERROR_ERRNO;
        goto fail;
    }

    if (moduleweb_file_open_read_utf8(&stream->file.file, filename)) {
        goto fail_filename;
    }

    if (moduleweb_file_lock(stream->file.file)) {
        goto fail_file;
    }

    if (moduleweb_file_get_size(stream->file.file, &stream->file.size)) {
        goto fail_file_locked;
    }

    stream->file.pos = 0;

    stream->buffer = malloc(MODULEWEB_INSTREAM_BUFFER_SIZE);
    stream->buffer_index = 0;
    stream->buffer_size = 0;

    stream->streaming_mode = MODULEWEB_STREAMING_FILE;

    return 0;

    fail_file_locked:
    moduleweb_file_unlock(stream->file.file);

    fail_file:
    moduleweb_file_close(&stream->file.file);

    fail_filename:
    free(stream->file.name);

    fail:
    return 1;
}

void moduleweb_instream_close(moduleweb_instream* stream) {
    if (stream->streaming_mode != MODULEWEB_STREAMING_FILE) return;

    if (moduleweb_file_unlock(stream->file.file)) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }

    if (moduleweb_file_close(&stream->file.file)) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }

    free(stream->file.name);
    free(stream->buffer);
}

int moduleweb_instream_open_buffer(moduleweb_instream* stream, const u8* buffer, u64 size, bool own_buffer) {
    stream->streaming_mode = MODULEWEB_STREAMING_MEMORY;
    stream->memory.ptr = buffer;
    stream->memory.size = size;
    stream->memory.pos = 0;

    stream->buffer = own_buffer ? buffer : NULL;

    return 0;
}

void moduleweb_instream_close_buffer(moduleweb_instream* stream) {
    if (stream->streaming_mode != MODULEWEB_STREAMING_MEMORY) return;

    if (stream->buffer != NULL) free(stream->buffer);
}

const char* moduleweb_instream_strerror(moduleweb_instream* stream) {
    if (stream->moduleweb_errno == MODULEWEB_ERROR_ERRNO) {
        return strerror(stream->sys_errno);
    }

    return moduleweb_strerror(stream->moduleweb_errno);
}

bool moduleweb_instream_is_eof(moduleweb_instream* stream) {
    return stream->file.pos == stream->file.size;
}

int moduleweb_instream_read_bytes(moduleweb_instream* stream, u8* res, u64 count) {
    if (stream->streaming_mode == MODULEWEB_STREAMING_FILE) {
        u64 bytes_read = 0;

        while (bytes_read < count) {
            if (stream->buffer_index >= stream->buffer_size) {
                u64 byte_count;
                if (moduleweb_file_read(&stream->file.file, stream->buffer, MODULEWEB_INSTREAM_BUFFER_SIZE, &byte_count)) {
                    stream->moduleweb_errno = MODULEWEB_ERROR_FILE_READ_FAIL;
                    return 1;
                }

                if (byte_count == 0) {
                    stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
                    return 1;
                }

                stream->buffer_index = 0;
                stream->buffer_size = byte_count;

                stream->file.pos += byte_count;
            }

            u64 to_copy = count - bytes_read;
            u64 available = stream->buffer_size - stream->buffer_index;
            if (to_copy > available) to_copy = available;

            memcpy(res + bytes_read, stream->buffer + stream->buffer_index, to_copy);
            bytes_read += to_copy;
            stream->buffer_index += to_copy;
        }

        return 0;
    } else if (stream->streaming_mode == MODULEWEB_STREAMING_MEMORY) {
        if (stream->memory.pos + count > stream->memory.size) {
            stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
            return 1;
        }

        memcpy(res, stream->memory.ptr + stream->memory.pos, count);

        stream->memory.pos += count;

        return 0;
    }

    return 1;
}

int moduleweb_instream_skip(moduleweb_instream* stream, u64 amount) {
    if (stream->streaming_mode == MODULEWEB_STREAMING_FILE) {
        if (stream->file.pos + amount > stream->file.size) {
            stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF;
            return 1;
        }

        u64 buffer_remaining = stream->buffer_size - stream->buffer_index;
        if (amount <= buffer_remaining) {
            stream->buffer_index += amount;
            stream->file.pos += amount;

            if (moduleweb_file_seek(&stream->file.file, stream->file.pos)) {
                stream->moduleweb_errno = MODULEWEB_ERROR_FILE_READ_FAIL;
                return 1;
            }

            return 0;
        }

        amount -= buffer_remaining;
        stream->file.pos += buffer_remaining;
        stream->buffer_index = stream->buffer_size;

        if (moduleweb_file_seek(&stream->file.file, stream->file.pos + amount)) {
            stream->moduleweb_errno = MODULEWEB_ERROR_FILE_SEEK_FAIL;
            return 1;
        }

        stream->file.pos += amount;
        stream->buffer_index = stream->buffer_size = 0; // reset buffer

        return 0;
    } else if (stream->streaming_mode == MODULEWEB_STREAMING_MEMORY) {
        stream->memory.pos += amount;
        return 0;
    }

    return 1;
}

u8* moduleweb_instream_pointer(moduleweb_instream* stream) {
    if (stream->streaming_mode == MODULEWEB_STREAMING_FILE) {
        return &stream->buffer[stream->buffer_index];
    } else if (stream->streaming_mode == MODULEWEB_STREAMING_MEMORY) {
        return &stream->memory.ptr[stream->memory.pos];
    }

    return NULL;
}

//
// OUT STREAM
//

int moduleweb_outstream_init(moduleweb_outstream* stream, const char* filename) {
    stream->file.name = strdup(filename);
    if (stream->file.name == NULL) {
        stream->sys_errno = errno;
        stream->moduleweb_errno = MODULEWEB_ERROR_ERRNO;
        goto fail;
    }

    if (moduleweb_file_open_write(&stream->file.file, filename)) {
        goto fail_filename;
    }

    if (moduleweb_file_lock(stream->file.file)) {
        goto fail_file;
    }

    stream->buffer = malloc(MODULEWEB_OUTSTREAM_BUFFER_SIZE);
    stream->buffer_index = 0;

    stream->streaming_mode = MODULEWEB_STREAMING_FILE;

    return 0;

    fail_file:
    moduleweb_file_close(&stream->file.file);

    fail_filename:
    free(stream->file.name);

    stream->sys_errno = errno;
    stream->moduleweb_errno = MODULEWEB_ERROR_ERRNO;

    fail:
    return 1;
}

void moduleweb_outstream_uninit(moduleweb_outstream* stream) {
    if (stream->streaming_mode != MODULEWEB_STREAMING_FILE) return;

    if (moduleweb_outstream_flush(stream)) {
        fprintf(stderr, "error: %s\n", moduleweb_outstream_strerror(stream));
    }

    if (moduleweb_file_unlock(stream->file.file)) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }

    if (moduleweb_file_close(&stream->file.file)) {
        fprintf(stderr, "error: %s\n", strerror(errno));
    }

    free(stream->file.name);
    free(stream->buffer);
}

int moduleweb_outstream_init_buffer(moduleweb_outstream* stream, u8* buffer, u64 size) {
    stream->streaming_mode = MODULEWEB_STREAMING_MEMORY;
    stream->memory.ptr = buffer;
    stream->memory.size = size;
    stream->memory.pos = 0;

    return 0;
}

int moduleweb_outstream_reopen_buffer(moduleweb_outstream* stream, PARAM_MUTATED u8* new_buffer, u64 size) {
    stream->memory.ptr = new_buffer;
    stream->memory.size = size;

    return 0;
}

void moduleweb_outstream_uninit_buffer(moduleweb_outstream* stream) {
    if (stream->streaming_mode != MODULEWEB_STREAMING_MEMORY) return;

    if (moduleweb_outstream_flush(stream)) {
        fprintf(stderr, "error: %s\n", moduleweb_outstream_strerror(stream));
    }
}

const char* moduleweb_outstream_strerror(moduleweb_outstream* stream) {
    if (stream->moduleweb_errno == MODULEWEB_ERROR_ERRNO) {
        return strerror(stream->sys_errno);
    }

    return moduleweb_strerror(stream->moduleweb_errno);
}

int moduleweb_outstream_write_bytes(moduleweb_outstream* stream, const u8* data, u64 size) {
    if (stream->streaming_mode == MODULEWEB_STREAMING_FILE) {
        u64 bytes_written = 0;

        while (bytes_written < size) {
            if (stream->buffer_index >= MODULEWEB_OUTSTREAM_BUFFER_SIZE) {
                if (moduleweb_outstream_flush(stream)) {
                    return 1;
                }
            }

            u64 to_copy = size - bytes_written;
            u64 available = MODULEWEB_OUTSTREAM_BUFFER_SIZE - stream->buffer_index;
            if (to_copy > available) to_copy = available;

            memcpy(stream->buffer + stream->buffer_index, data + bytes_written, to_copy);
            bytes_written += to_copy;
            stream->buffer_index += to_copy;
        }

        return 0;
    } else if (stream->streaming_mode == MODULEWEB_STREAMING_MEMORY) {
        if (stream->memory.pos + size > stream->memory.size) {
            stream->moduleweb_errno = MODULEWEB_ERROR_UNEXPECTED_EOF; // most appropriate error for reaching end of the "file" buffer
            return 1;
        }

        memcpy(stream->memory.ptr + stream->memory.pos, data, size);

        stream->memory.pos += size;

        return 0;
    }

    return 1;
}

int moduleweb_outstream_flush(moduleweb_outstream* stream) {
    if (stream->buffer_index == 0) return 0; // no buffer, nothing to be flushed

    if (stream->streaming_mode == MODULEWEB_STREAMING_FILE) {
        u64 bytes_written;

        if (moduleweb_file_write(&stream->file.file, stream->buffer, stream->buffer_index, &bytes_written)) {
            stream->moduleweb_errno = MODULEWEB_ERROR_FILE_WRITE_FAIL;
            stream->sys_errno = errno;
            return 1;
        }

        if (bytes_written != stream->buffer_index) { // not all bytes were written
            stream->moduleweb_errno = MODULEWEB_ERROR_FILE_WRITE_FAIL;
            return 1;
        }

        stream->buffer_index = 0;

        return 0;
    } else if (stream->streaming_mode == MODULEWEB_STREAMING_MEMORY) {
        return 0;
    }

    return 1;
}