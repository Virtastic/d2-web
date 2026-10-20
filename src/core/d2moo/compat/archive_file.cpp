// Prior art: D2MOO D2Hell/src/Archive.cpp, forwarding archive streams to Fog.
// Archive context and diagnostic logging remain compatibility-layer concerns.
#include <Archive.h>
#include "../../fog/fog_file_service.h"

uint32_t ARCHIVE_GetLastError() { return d2::nativeFileLastError(); }
BOOL ARCHIVE_OpenFile(HD2ARCHIVE, const char* path, HSFILE* out, BOOL) {
    void* file = nullptr;
    const auto result = FOG_FOpenFile(path, out ? &file : nullptr);
    if (out) *out = reinterpret_cast<HSFILE>(file);
    return result;
}
void ARCHIVE_CloseFile(HD2ARCHIVE, HSFILE file) {
    FOG_FCloseFile(file);
}
