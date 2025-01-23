#include "moduleweb/constant_info.h"

int moduleweb_constant_info_init(moduleweb_constant_info* info, moduleweb_instream* stream);

void moduleweb_constant_info_uninit(moduleweb_constant_info* info);

int moduleweb_constant_info_ascii_compare(moduleweb_constant_ascii_info* s1, const char* s2) {
    u32 s2_length = strlen(s2);

    if (s1->length != s2_length) {

    }
}