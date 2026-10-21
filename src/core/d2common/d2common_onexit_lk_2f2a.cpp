// src/core/d2common/d2common_onexit_lk_2f2a.cpp -- 1.13c D2Common.dll __onexit_lk (VC7.1 CRT).
// Base 0x6FD50000. __msize (0x6234) and _realloc (0x6082) are other rows, passed in.
#include <cstdint>

struct D2CommonOnexitEnv {
    uint32_t  (*msize)(void* p);                   // __msize
    void*     (*realloc_)(void* p, uint32_t n);    // FUN_6fd56082
};
struct D2CommonOnexitTable {
    uint32_t* end;    // DAT_6fdf2850 next free slot
    uint32_t* base;   // DAT_6fdf2854 table start
};

// 1.13c D2Common+0x02F2A (128 B): append `fn` (the caller's EDI) to the atexit table. If the block has
// no room for one more dword, grow by min(size,0x800) bytes; on failure retry with size+0x10; on
// second failure return without appending. Realloc moves base; end is rebased by the old offset.
extern "C" void D2Common_2F2A_OnexitLk(uint32_t fn, D2CommonOnexitTable* t, const D2CommonOnexitEnv* env)
{
    uint32_t sz = env->msize(t->base);
    if (sz < (uint32_t)((uintptr_t)t->end - (uintptr_t)t->base) + 4) {
        uint32_t add = sz < 0x800 ? sz : 0x800;
        uint32_t* p = (uint32_t*)env->realloc_(t->base, sz + add);
        if (!p) {
            p = (uint32_t*)env->realloc_(t->base, sz + 0x10);
            if (!p) return;
        }
        t->end = p + (((uintptr_t)t->end - (uintptr_t)t->base) >> 2);
        t->base = p;
    }
    *t->end++ = fn;
}
