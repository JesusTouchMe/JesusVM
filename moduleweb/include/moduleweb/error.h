#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ERROR_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif


typedef enum moduleweb_errno {
    MODULEWEB_ERROR_ERRNO,

    MODULEWEB_ERROR_FILE_READ_FAIL,
    MODULEWEB_ERROR_FILE_WRITE_FAIL,
    MODULEWEB_ERROR_FILE_SEEK_FAIL,

    MODULEWEB_ERROR_EXPECTED_EOF,
    MODULEWEB_ERROR_UNEXPECTED_EOF,
    MODULEWEB_ERROR_BAD_MAGIC,
} moduleweb_errno;

const char* moduleweb_strerror(moduleweb_errno error);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_ERROR_H
