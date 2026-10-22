// src/core/d2cmp/d2cmp_batch_b61.cpp -- alpha-lane batch: D2CMP CRT rows from the cached Ghidra decompiles.
// No spec, no vector. Globals/callees of the original image are passed in via env structs.
// Skipped (SEH / register args / direct Win32): 3B03 3CB5 3E8E 4004 41CA 44DB 4647 46E9 4825.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <initializer_list>

struct D2CmpEnvB61 {
    void* (*malloc_)(size_t);              // 0x2E8D _malloc
    void (*free_)(void*);                  // 0x2EBE _free
    size_t (*strlen_)(const char*);        // 0x6D10
    char* (*strcpy_)(char*, const char*);  // 0x6D90
    void (*initmbctable)();                // 0x6120
    void (*deleteCritSec)(void*);          // DeleteCriticalSection
    int32_t* mbctableInit;                 // DAT_6ff14fb4
    uint32_t* ioTable;                     // DAT_6ff14ea0, 64 entries of 32-bit block ptrs
    const char** envBlock;                 // DAT_6fe3464c
    uint32_t** envp;                       // DAT_6fe34624
    int32_t* envpReady;                    // DAT_6ff14fa8
};

// 1.13c D2CMP+0x3E0A
void d2cmp_3e0a(uint32_t* ptd, uint32_t initaddr /* &DAT_6fe321c0 */) {
    ptd[0x15] = initaddr; // _initaddr
    ptd[5] = 1;           // _holdrand
}

// 1.13c D2CMP+0x43C8
void d2cmp_43c8(const D2CmpEnvB61* e) {
    for (int i = 0; i < 64; i++) {
        uint32_t blk = e->ioTable[i];
        if (!blk) continue;
        for (uint32_t p = blk; p < blk + 0x480; p += 0x24) {
            if (*(int32_t*)(uintptr_t)(p + 8) != 0) e->deleteCritSec((void*)(uintptr_t)(p + 0xc));
        }
        e->free_((void*)(uintptr_t)blk);
        e->ioTable[i] = 0;
    }
}

// 1.13c D2CMP+0x4414
int d2cmp_4414(const D2CmpEnvB61* e) {
    if (*e->mbctableInit == 0) e->initmbctable();
    int n = 0;
    const char* p = *e->envBlock;
    if (!p) return -1;
    for (; *p; p += e->strlen_(p) + 1)
        if (*p != '=') n++;
    char** out = (char**)e->malloc_(n * sizeof(char*) + sizeof(char*));
    p = *e->envBlock;
    *e->envp = (uint32_t*)out;
    if (!out) return -1;
    for (;;) {
        if (!*p) {
            e->free_((void*)*e->envBlock);
            *e->envBlock = nullptr;
            *out = nullptr;
            *e->envpReady = 1;
            return 0;
        }
        size_t len = e->strlen_(p);
        if (*p != '=') {
            char* s = (char*)e->malloc_(len + 1);
            *out = s;
            if (!s) {
                e->free_(*e->envp);
                *e->envp = nullptr;
                return -1;
            }
            e->strcpy_(s, p);
            out++;
        }
        p += len + 1;
    }
}
