#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_READER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_READER_H

#include "moduleweb/module_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_reader {
    UNOWNED_OBJECT moduleweb_instream* stream;
    bool is_accepted;
} moduleweb_reader;

int moduleweb_reader_init(moduleweb_reader* reader, PARAM_REFERENCED moduleweb_instream* stream);
void moduleweb_reader_uninit(moduleweb_reader* reader);

int moduleweb_reader_accept(moduleweb_reader* reader, PARAM_MUTATED moduleweb_module_info* module, const char* name);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_READER_H
