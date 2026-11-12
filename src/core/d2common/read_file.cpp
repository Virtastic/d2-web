#include "../tables/shared.h"
#include "read_file.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <vector>

// Prior art: D2MOO's D2Hell/Archive.cpp `ARCHIVE_AllocateBufferAndReadFile`, used by
// D2Common's AnimTbls.cpp and Drlg/DrlgPreset.cpp, establishes the archive-handle, path,
// and optional DWORD-size-out contract.
// 1.13c D2Common+0x09900 -- EAX = file name, two stack words (the first unused, the second the optional size-out pointer),
// ret 8. The second argument's own stack slot is the handle cell the open call fills, so an open that reports success
// without writing it leaves the size-out pointer there as the handle. Open fails: the last error is read, one line is
// logged, 0 is returned and the size-out is left alone. Otherwise the size is read, size + 800 bytes are allocated
// (line 0xCF; a null buffer returns 0 with the file left open), the file is read, a zero handle is fatal (line 0x49),
// the file is closed, the size goes out when asked for and the buffer is returned.
void* d2common_read_file(const char* name, uint32_t* sizeOut, const D2ReadFileEnv* e) {
    uintptr_t cell = (uintptr_t)sizeOut;
    if (!e->open(name, &cell)) {
        e->openFailed(name);
        return nullptr;
    }
    uint32_t scratch = 0;
    const uint32_t size = e->size(cell, &scratch);
    void* buf = e->alloc(size + 0x320, 0xCF);
    if (!buf) return nullptr;
    e->read(cell, size, buf);
    if (!cell) e->fatal(0x49);
    e->close(cell);
    if (sizeOut) *sizeOut = size;
    return buf;
}

// The play path's callees: the file comes whole from the MPQ VFS bridge, the handle is its byte vector.
namespace {
using Bytes = std::vector<uint8_t>;
int vfsOpen(const char* name, uintptr_t* cell) {
    if (!name) return 0;
    Bytes* b = new Bytes;
    if (!d2::sharedVfs().read(name, *b)) { delete b; return 0; }
    *cell = (uintptr_t)b;
    return 1;
}
void vfsOpenFailed(const char*) {}
uint32_t vfsSize(uintptr_t h, uint32_t*) { return (uint32_t)((Bytes*)h)->size(); }
void* vfsAlloc(uint32_t bytes, int) { return std::calloc(bytes, 1); }
void vfsRead(uintptr_t h, uint32_t size, void* buf) { if (size) std::memcpy(buf, ((Bytes*)h)->data(), size); }
void vfsFatal(int) { std::abort(); }
void vfsClose(uintptr_t h) { delete (Bytes*)h; }
const D2ReadFileEnv kVfsEnv{ vfsOpen, vfsOpenFailed, vfsSize, vfsAlloc, vfsRead, vfsFatal, vfsClose };
}

extern "C" void* D2ReadFile(void*, const char* filename, uint32_t* size) {
    return d2common_read_file(filename, size, &kVfsEnv);
}
