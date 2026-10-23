// 1.13c D2sound+0x6360 -- load a file into a Fog-allocated buffer (callers: 0x8320).
// The decompile lost the register/stack args of every callee (Fog ordinals 10102/10105/10042/10103,
// helper 0x6070); each is a host seam, the control flow and error paths are retail's.
#include <cstdint>
#include <unistd.h>

struct D2sound6360Env {
    int  (*open)(void* name, int* hFile);             // Ordinal_10102; 0 = failed (GetLastError + log 10029)
    int  (*size)(int hFile);                          // Ordinal_10105; 0 = zero-length / failed
    void*(*alloc)(int size, int tag);                 // Ordinal_10042(0xCF, 0) -- 0 = failed
    void (*read6070)(int hFile, void* buf, int sz);   // FUN_6070: reads, fatal (_exit(-1)) on short read
    void (*close)(int hFile);                         // Ordinal_10103
    void (*fatal)(uint32_t strId);                    // Ordinal_10265 -> Ordinal_10024 -> _exit(-1)
    void (*logOpenFail)();                            // GetLastError; Ordinal_10029("Error opening file: %s")
};

// Returns the buffer (0 on open/alloc failure); *outSize receives the file size if non-null.
void* D2sound_6360(void* name, int* outSize, const D2sound6360Env* e) {
    int h = 0;
    if (!e->open(name, &h)) { e->logOpenFail(); return nullptr; }
    if (h == 0) { e->fatal(0x59); _exit(-1); }
    int sz = e->size(h);
    if (sz == 0) { e->fatal(0x5F); _exit(-1); }       // retail also Ordinal_276-reads the name first
    void* buf = e->alloc(sz, 0xCF);
    if (!buf) return nullptr;                         // handle not closed on this path (as retail)
    e->read6070(h, buf, sz);
    e->close(h);
    if (outSize) *outSize = sz;
    return buf;
}
