#include <cstdint>
#include <cstdlib>

// Prior art used: PlugY Commons/D2Funcs.h names Fog's archive and allocation
// imports. D2MOO has no client module. Cached 1.13c decompiles cross-checked
// against x86 instructions: D810's decompile loses the register arguments.
// Services are explicit import/helper bindings, not a substitute global store.
// Handles and allocation results use native pointers; sizes stay retail u32.
#include "file_services.h"

// 1.13c D2Client+0xD6A0
extern "C" uint32_t d2_client_d6a0(const D2ClientFileServices& api, const char* path,
                      void** file, uint32_t allowMissing) {
    if (api.open(path, file)) return 1;
    const uint32_t error = api.lastError();
    if (!allowMissing || error != 2)
        api.reportOpenError("Error opening file: %s", path);
    return 0;
}

// 1.13c D2Client+0xD810
extern "C" void* d2_client_d810(const D2ClientFileServices& api, const char* path,
                    uint32_t* sizeOut) {
    void* file;
    if (!api.open(path, &file)) {
        api.lastError();
        api.reportOpenError("Error opening file: %s", path);
        return nullptr;
    }
    uint32_t sizeHigh;
    const uint32_t size = d2_client_d610(file, &sizeHigh, &api);
    void* buffer = api.allocate(size + uint32_t{0x320},
                               "..\\Source\\D2Hell\\SRC\\Archive.cpp", 0xCF, 0);
    // Retail returns directly here: it does not close the open file on OOM.
    if (!buffer) return nullptr;
    d2_client_d520(file, size, buffer, &api);
    if (!file) {
        api.fatal(0x49);
        std::abort(); // retain the nonreturning contract even with a bad binding
    }
    api.close(file);
    if (sizeOut) *sizeOut = size;
    return buffer;
}
