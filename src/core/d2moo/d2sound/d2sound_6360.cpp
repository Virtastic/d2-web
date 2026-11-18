// 1.13c D2sound+0x6360 -- load a file into a Fog-allocated buffer (callers: 0x8320).
// EAX = file name, stack (unused word, optional out pointer for the size), ret 8. The callees are fastcall import thunks; the seams below
// carry exactly their retail arguments (disassembly 0x6360..0x6474), the control flow and fatal paths are retail's.
#include <cstdint>
#include <unistd.h>

struct D2sound6360Env {
    int32_t  (*open)(const void* name, uint32_t* hFile);        // 0x5F6E (Ordinal_10102): ECX = name, EDX = &hFile
    uint32_t (*size)(uint32_t hFile, uint32_t* high);           // 0x5FDE (Ordinal_10105): ECX = hFile, EDX = &high dword; EAX = size
    uint32_t (*alloc)(uint32_t bytes, uint32_t tagStr, uint32_t tag, uint32_t zero);   // 0x5F62: ECX = size + 0x320, EDX = 0x6F9C0928, stack (0xCF, 0)
    void     (*read6070)(uint32_t hFile, uint32_t size, uint32_t buf);   // 0x6070: ESI = hFile, EDI = size, stack buf
    void     (*close)(uint32_t hFile);                          // 0x5F74 (Ordinal_10103): ECX = hFile
    void     (*logOpenFail)(const void* name);                  // GetLastError; 0x5F56 (Ordinal_10029)(0x6F9C094C "Error opening file: %s", name)
};

// Returns the buffer (0 on open/alloc failure); *outSize receives the file size if non-null.
void* D2sound_6360(void* name, int* outSize, const D2sound6360Env* e) {
    uint32_t h = 0, high = 0;
    if (!e->open(name, &h)) { e->logOpenFail(name); return nullptr; }
    if (h == 0) _exit(-1);                                      // string 0x59
    const uint32_t sz = e->size(h, &high);
    if (sz == 0) _exit(-1);                                     // retail also Ordinal_276-copies the name first; string 0x5F
    const uint32_t buf = e->alloc(sz + 0x320, 0x6F9C0928u, 0xCF, 0);
    if (!buf) return nullptr;                                   // handle not closed on this path (as retail)
    e->read6070(h, sz, buf);
    if (h == 0) _exit(-1);                                      // string 0x49 (the handle re-test after the read)
    e->close(h);
    if (outSize) *outSize = (int)sz;
    return (void*)(uintptr_t)buf;
}
