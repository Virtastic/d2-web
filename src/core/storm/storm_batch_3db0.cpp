// src/core/storm/storm_batch_3db0.cpp -- Storm 1.13c rows 0x3DB0 0x4DA0 0x56ED 0x5FC0 0x6000.
// Skipped (no cached decompile): 0x45E0 0x4660 0x4CE0 0x4DE0 0x55D9 0x5CC0 0x5D30.
// Register args Ghidra shows as in_EAX/unaff_ESI are explicit parameters; callees are seams.
#include <cstdint>
#include <cstddef>
#include <cctype>

namespace {
typedef uint32_t u32;

struct Env3db0 {  // 0x1C760 realloc, 0x1C580 alloc, 0x1B960 free (name "...", line -2)
    void* (*realloc_)(void* p, u32 n, const char* nm, int line, u32 fl);
    void* (*alloc_)(u32 n, const char* nm, int line, u32 fl);
    void (*free_)(void* p, const char* nm, int line);
};
}  // namespace

// 1.13c STORM+0x3DB0 -- grow buffer object this={size,cap,ptr} to n bytes (eax=n, esi=this)
extern "C" void storm_3db0(u32* self, u32 n, const Env3db0* e, const char* nm) {
    uint8_t* old = (uint8_t*)(uintptr_t)self[2];
    self[0] = n;
    uint8_t* p = (uint8_t*)e->realloc_(old, n, nm, -2, 0x10);
    // ponytail: self[2] is a u32 slot (wasm32 layout); wasm64 pointers must fit in 4GB, else abort rather than truncate.
    if ((uintptr_t)p >> 32) __builtin_trap();
    self[2] = (u32)(uintptr_t)p;
    if (p) return;
    p = (uint8_t*)e->alloc_(n, nm, -2, 0);
    if ((uintptr_t)p >> 32) __builtin_trap();
    self[2] = (u32)(uintptr_t)p;
    if (!old) return;
    if (self[1] <= n) n = self[1];
    for (u32 i = 0; i < n; i++)
        if (p + i) p[i] = old[i];
    e->free_(old, nm, -2);
}

// 1.13c STORM+0x4DA0 -- destroy GDI object this (esi); param_1 supplies the fallback record at +4
extern "C" u32 storm_4da0(int param_1, int* self, void (*dtor_4d60)(int*), void (*free_)(void*, const char*, int)) {
    int* r = self ? self : (int*)(uintptr_t)(param_1 + 4);
    u32 v = (u32)r[1];
    dtor_4d60(self);
    free_(self, "AUSGDIOBJ", -2);
    return (int)v < 1 ? 0 : v;
}

// 1.13c STORM+0x56ED -- _strupr; the locale branch (wlocale != 0 -> LCMapString 0x200) goes through a seam
extern "C" char* storm_56ed(char* s, bool (*lcMapUpper)(char*)) {
    if (lcMapUpper && lcMapUpper(s)) return s;
    for (char* p = s; *p; p++)
        if (*p > '`' && *p < '{') *p -= 0x20;
    return s;
}

// 1.13c STORM+0x5FC0 -- _strpbrk (bitmap of the control set)
extern "C" char* storm_5fc0(char* str, const char* ctl) {
    uint8_t m[32] = {};
    for (; *ctl; ctl++) m[(uint8_t)*ctl >> 3] |= 1 << ((uint8_t)*ctl & 7);
    for (; *str; str++)
        if (m[(uint8_t)*str >> 3] >> ((uint8_t)*str & 7) & 1) return str;
    return nullptr;
}

// 1.13c STORM+0x6000 -- __ArrayUnwind: run dtor over count elements (edi = element ptr, advanced by the dtor seam)
extern "C" void storm_6000(void* edi, unsigned, int count, void (*dtor)(void*)) {
    while (--count >= 0) dtor(edi);
}
