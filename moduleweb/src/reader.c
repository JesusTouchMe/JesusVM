#include <stdio.h>
#include "moduleweb/reader.h"

int moduleweb_reader_init(moduleweb_reader* reader, moduleweb_instream* stream) {
    reader->stream = stream;
    reader->is_accepted = false;
}

void moduleweb_reader_uninit(moduleweb_reader* reader) {
    reader->is_accepted = true;
}

int moduleweb_reader_accept(moduleweb_reader* reader, moduleweb_module_info* module) {
    if (reader->is_accepted) {
        return -1;
    }

    if (moduleweb_module_info_init(module, reader->stream)) {
        fprintf(stderr, "error: %s\n", moduleweb_instream_strerror(reader->stream));
        return 1;
    }

    return 0;
}