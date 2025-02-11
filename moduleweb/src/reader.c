#include <stdio.h>
#include "moduleweb/reader.h"

int moduleweb_reader_init(moduleweb_reader* reader, moduleweb_instream* stream) {
    reader->stream = stream;
    reader->is_accepted = false;

    return 0;
}

void moduleweb_reader_uninit(moduleweb_reader* reader) {
    reader->is_accepted = true;
}

int moduleweb_reader_accept(moduleweb_reader* reader, moduleweb_module_info* module, const char* name) {
    if (reader->is_accepted) {
        return -1;
    }

    if (moduleweb_module_info_init(module, name, strlen(name), reader->stream)) {
        fprintf(stderr, "error: %s (%d)\n", moduleweb_instream_strerror(reader->stream), reader->stream->moduleweb_errno);
        return 1;
    }

    return 0;
}