// src/core/d2cmp/d2cmp_batch_crt_b54.cpp -- 1.13c D2CMP.dll MSVC 2003 CRT rows (alpha batch, marker only).
// Not ported: 0x6E88 MessageBox, 0x56A5 small-block heap, 0x26E7 type_info dtor, 0x2AA2/0x2C23 DllMain
// CRT entry, 0x2A01 (SEH + lock thunks), 0x2D3A/0x2D83 (critical sections), 0x27A3 (init-table walk over 32-bit data): no faithful wasm64 form or Win32-only.
#include <cstdint>
#include <cstdlib>
#include <cstddef>

// DAT_6ff14fb0 / DAT_6ff14fac: the atexit table base and next-free slot.
static void** g_onexitBegin;
static void** g_onexitEnd;
static size_t g_onexitCap;   // stands in for __msize

// 1.13c D2CMP+0x3090 (135 bytes): _strcmp -- unsigned byte compare, -1/0/1.
// 1.13c D2CMP+0x3090
extern "C" int d2cmp_3090(const char* a, const char* b) {
    for (;; ++a, ++b) {
        uint8_t x = (uint8_t)*a, y = (uint8_t)*b;
        if (x != y) return x < y ? -1 : 1;
        if (!x) return 0;
    }
}

// 1.13c D2CMP+0x29D9 (40 bytes): ___onexitinit -- 128-byte table; 0x18 on allocation failure.
// 1.13c D2CMP+0x29D9
extern "C" int d2cmp_29d9(void) {
    g_onexitBegin = (void**)malloc(0x80);
    if (!g_onexitBegin) return 0x18;
    g_onexitCap = 0x80;
    *g_onexitBegin = nullptr;
    g_onexitEnd = g_onexitBegin;
    return 0;
}

// 1.13c D2CMP+0x2959 (128 bytes): __onexit_lk -- append fn, growing by min(size,0x800) else size+0x10.
// 1.13c D2CMP+0x2959
extern "C" void d2cmp_2959(void* fn) {
    size_t sz = g_onexitCap;   // __msize of the table
    if (sz < (size_t)((char*)g_onexitEnd - (char*)g_onexitBegin) + sizeof(void*)) {
        size_t grow = sz < 0x800 ? sz : 0x800;
        void** n = (void**)realloc(g_onexitBegin, sz + grow);
        size_t cap = sz + grow;
        if (!n) { n = (void**)realloc(g_onexitBegin, sz + 0x10); cap = sz + 0x10; }
        if (!n) return;
        g_onexitEnd = n + (g_onexitEnd - g_onexitBegin);
        g_onexitBegin = n;
        g_onexitCap = cap;
    }
    *g_onexitEnd++ = fn;
}
