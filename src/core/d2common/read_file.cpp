#include "../tables/shared.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// Prior art: D2MOO's D2Hell/Archive.cpp `ARCHIVE_AllocateBufferAndReadFile`, used by
// D2Common's AnimTbls.cpp and Drlg/DrlgPreset.cpp, establishes the archive-handle, path,
// and optional DWORD-size-out contract. The 1.13c body at +0x9900 confirms open, size,
// allocate, read, close, and return-buffer behavior; this uses the project's MPQ VFS bridge.
// 1.13c D2Common+0x09900
extern "C" void* D2ReadFile(void*, const char* filename, uint32_t* size) {
    if (size) *size = 0;
    if (!filename) return nullptr;

    std::vector<uint8_t> contents;
    if (!d2::sharedVfs().read(filename, contents)) return nullptr;

    // D2MOO's archive reader leaves 800 bytes beyond the payload; keep that parser slack.
    void* buffer = std::calloc(contents.size() + 800, 1);
    if (!buffer) return nullptr;
    if (!contents.empty()) std::memcpy(buffer, contents.data(), contents.size());
    if (size) *size = static_cast<uint32_t>(contents.size());
    return buffer;
}
