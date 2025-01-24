#include "moduleweb/writer.h"

int moduleweb_writer_init(moduleweb_writer* writer, moduleweb_outstream* stream) {
    writer->stream = stream;
    writer->is_accepted = false;
}

void moduleweb_writer_uninit(moduleweb_writer* writer) {
    writer->is_accepted = true;
}

int moduleweb_writer_accept(moduleweb_writer* writer, moduleweb_module_info* module) {
    if (writer->is_accepted) {
        return -1;
    }

    if (moduleweb_module_info_emit_bytes(module, writer->stream)) {
        return 1;
    }

    if (moduleweb_outstream_flush(writer->stream)) {
        return 1;
    }

    return 0;
}
