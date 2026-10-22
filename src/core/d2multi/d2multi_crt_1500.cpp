// src/core/d2multi/d2multi_crt_1500.cpp -- more D2Multi.dll CRT leaves (memmove, str*, rand). C locale and one
// global rand seed (per-thread _ptiddata collapses to a global on single-threaded wasm), as in d2multi_crt_1000.
// Not ported: _exit/__exit/__cexit (need unported doexit 0x13F7), ___tolower_mt (takes a locinfo struct).
#include <cstdint>
#include <cstddef>
#include <cstring>

static uint32_t g_holdrand = 1;  // _ptiddata->_holdrand

// 1.13c D2Multi+0x1560  _memmove: overlap-safe copy, returns dst
void* d2multi_memmove(void* dst, const void* src, size_t n) { return memmove(dst, src, n); }

// 1.13c D2Multi+0x18B0  _strchr: first match, terminator included
char* d2multi_strchr(const char* s, int c) {
    for (;; ++s) {
        if (*s == (char)c) return (char*)s;
        if (*s == 0) return nullptr;
    }
}

// 1.13c D2Multi+0x1A36  _tolower (C locale)
int d2multi_tolower(int c) { return (c > 0x40 && c < 0x5b) ? c + 0x20 : c; }

// 1.13c D2Multi+0x14EB  __stricmp (C locale): difference of lowered bytes at first mismatch/NUL
int d2multi_stricmp(const char* a, const char* b) {
    int x, y;
    do {
        x = d2multi_tolower((uint8_t)*a++);
        y = d2multi_tolower((uint8_t)*b++);
    } while (x && x == y);
    return x - y;
}

// 1.13c D2Multi+0x1A58  srand
void d2multi_srand(uint32_t seed) { g_holdrand = seed; }

// 1.13c D2Multi+0x1A65  _rand: LCG 214013/2531011, bits 16..30
int d2multi_rand(void) {
    g_holdrand = g_holdrand * 0x343fd + 0x269ec3;
    return (g_holdrand >> 16) & 0x7fff;
}

// 1.13c D2Multi+0x1A90  _strrchr: last match, terminator included
char* d2multi_strrchr(const char* s, int c) {
    const char* r = nullptr;
    for (;; ++s) {
        if (*s == (char)c) r = s;
        if (*s == 0) return (char*)r;
    }
}

// 1.13c D2Multi+0x1AC0  _strncat: append at most n chars, always NUL-terminate
char* d2multi_strncat(char* dst, const char* src, size_t n) {
    if (n == 0) return dst;
    char* p = dst + strlen(dst);
    while (n-- && (*p = *src++) != 0) ++p;
    *p = 0;
    return dst;
}
