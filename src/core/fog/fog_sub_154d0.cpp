// 1.13c Fog+0x154D0 -- sLoadSymbols(hProcess), stdcall: takes the module list from Fog+0x15470 (linked list,
// next at +0x20c, image path at +0, name at +0x104, base +0x204, size +0x208), logs "" and "Load symbols..."
// through the Fog+0x190A0 logger, then per module copies path and name into stack buffers and calls the
// imagehlp SymLoadModule pointer (Fog+0x56E40) with them; unless the Fog+0x32AC8 quiet flag is set it logs
// "Symbols Loaded for '%s'" (nonzero result) or "Error (%d) loading symbols for '%s'" with GetLastError.
// Returns the list head. The host has no imagehlp, so the module list, the loader and the logger are seams.
// The logger is cdecl and sees three stack words; on the first two calls the third is what retail's stack
// slot already held (a pointer into the frame, then the context string), passed here explicitly.
#include <cstdint>
#include <cstring>

struct FogSymSeams {
    void* u;
    uint32_t (*modules)(void* u, uint32_t hProcess);                       // Fog+0x15470
    void (*log)(void* u, uint32_t ctx, uint32_t fmt, uint32_t w2);           // Fog+0x190A0
    uint32_t (*symLoad)(void* u, uint32_t hProcess, uint32_t hFile, uint32_t image, uint32_t module,
                        uint32_t base, uint32_t size);                       // [Fog+0x56E40]
    uint32_t (*lastError)(void* u);
    const uint32_t* quiet;                                                   // Fog+0x32AC8
    uint32_t ctx, sEmpty, sLoad, sOk, sErr;                                  // Fog+0x268EC, +0x268EC, +0x27440, +0x27404, +0x2741C
};

// 1.13c Fog+0x154D0
extern "C" uint32_t fog_sub_154d0(const FogSymSeams* e, uint32_t hProcess) {
    char path[0x104], name[0x104];
    const uint32_t head = e->modules(e->u, hProcess);
    e->log(e->u, e->ctx, e->sEmpty, (uint32_t)(uintptr_t)path);
    e->log(e->u, e->ctx, e->sLoad, e->ctx);
    for (uint32_t node = head; node != 0;) {
        const uint8_t* n = (const uint8_t*)(uintptr_t)node;
        uint32_t next, base, size;
        memcpy(&next, n + 0x20c, 4);
        strcpy(path, (const char*)n);
        strcpy(name, (const char*)n + 0x104);
        memcpy(&base, n + 0x204, 4);
        memcpy(&size, n + 0x208, 4);
        const uint32_t r = e->symLoad(e->u, hProcess, 0, (uint32_t)(uintptr_t)path, (uint32_t)(uintptr_t)name, base, size);
        if (*e->quiet == 0) {
            if (r == 0) e->log(e->u, e->ctx, e->sErr, e->lastError(e->u));   // the name rides as a fourth word the 3-word record never shows
            else e->log(e->u, e->ctx, e->sOk, (uint32_t)(uintptr_t)name);
        }
        node = next;
    }
    return head;
}
