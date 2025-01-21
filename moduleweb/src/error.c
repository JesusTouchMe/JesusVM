#include "moduleweb/error.h"

#include <stdio.h>

const char* moduleweb_error_messages[] = {
        [MODULEWEB_ERROR_ERRNO] = "system error",
        [MODULEWEB_ERROR_EXPECTED_EOF] = "Expected end of file",
        [MODULEWEB_ERROR_UNEXPECTED_EOF] = "Unexpected end of file",
        [MODULEWEB_ERROR_BAD_MAGIC] = "Bad magic number",
};

const char* moduleweb_strerror(moduleweb_errno error) {
    if (error >= sizeof(moduleweb_error_messages) / sizeof(moduleweb_error_messages[0])) {
        return NULL;
    }

    return moduleweb_error_messages[error];
}
