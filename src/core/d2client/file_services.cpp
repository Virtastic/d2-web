// Prior art: D2MOO Fog.h owns diagnostics/allocation; the boot VFS owns streams.
#include "file_services.h"
#include "../fog/fog_file_service.h"
#include "../../assets/mpqvfs.h"
#include <Fog.h>
#include <cstdlib>
namespace d2 { MpqVfs& sharedVfs(); }
namespace {
int readSynchronous(void* f, void* p, uint32_t n, uint32_t* read) {
    return d2::sharedVfs().readFile(f, p, n, read);
}
int fileName(void* f, char* p, uint32_t capacity) {
    return d2::sharedVfs().fileName(f, p, capacity);
}
void fatal(uint32_t line) {
    // Compatibility diagnostic owner preserves the source/line. Fog #10265's
    // native helper is not claimed here; this is not an ordinal implementation.
    FOG_DisplayHalt("", "..\\Source\\D2Hell\\SRC\\Archive.cpp", line);
    std::exit(-1);
}
void report(const char* format, const char* path) { FOG_Trace(format, path); }
void* allocate(uint32_t n, const char* file, uint32_t line, uint32_t flags) {
    return FOG_Alloc(n, file, line, flags);
}
}
const D2ClientFileServices& d2_client_native_file_services() {
    static const D2ClientFileServices services{
        readSynchronous, FOG_FGetFileSize, FOG_FCloseFile, fileName, fatal,
        FOG_FOpenFile, d2::nativeFileLastError, report, allocate};
    return services;
}

void* d2_client_load_archive(const char* path, uint32_t* size) {
    return d2_client_d810(d2_client_native_file_services(), path, size);
}
