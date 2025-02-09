#ifndef JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_WRITER_H
#define JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_WRITER_H

#include "moduleweb/module_info.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct moduleweb_writer {
    UNOWNED_OBJECT moduleweb_outstream* stream;
    bool is_accepted;
} moduleweb_writer;

int moduleweb_writer_init(moduleweb_writer* writer, PARAM_REFERENCED moduleweb_outstream* stream);
void moduleweb_writer_uninit(moduleweb_writer* writer);

int moduleweb_writer_accept(moduleweb_writer* writer, PARAM_MUTATED moduleweb_module_info* module);

#ifdef __cplusplus
}
#endif

#endif //JESUSVM_MODULEWEB_INCLUDE_MODULEWEB_WRITER_H
