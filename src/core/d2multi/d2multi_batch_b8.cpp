// D2Multi locale/codepage helpers (MSVC CRT). Win32 calls are weak externs; wasm has no codepages.
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#define W __attribute__((weak))
extern int d2multi_GetLocaleInfoA(unsigned, unsigned, char*, int) W;
extern int d2multi_GetCPInfoMaxChar(unsigned) W;  // MaxCharSize, 0 on failure
extern int d2multi_MultiByteToWideChar(unsigned, unsigned, const char*, int, uint16_t*, int) W;
extern int d2multi_WideCharToMultiByte(unsigned, unsigned, const uint16_t*, int, char*, int, const char*, int*) W;
extern void* d2multi_heap_alloc(size_t) W;

// 1.13c D2Multi+0x6530
long d2multi_getLocaleCodePage(unsigned lcid) {
    char b[7] = {0};
    if (!d2multi_GetLocaleInfoA(lcid, 0x1004, b, 6)) return -1;
    return atol(b);
}

// 1.13c D2Multi+0x6577
char* d2multi_convertStringCP(unsigned from, unsigned to, const char* s, unsigned* cb, char* out, int outLen) {
    if (from == to) return nullptr;
    unsigned n = *cb;
    bool single = d2multi_GetCPInfoMaxChar(from) == 1 && d2multi_GetCPInfoMaxChar(to) == 1;
    int wn = single ? (n == 0xffffffffu ? (int)strlen(s) + 1 : (int)n) : d2multi_MultiByteToWideChar(from, 1, s, n, nullptr, 0);
    if (!wn) return nullptr;
    uint16_t* w = (uint16_t*)calloc(wn, 2);
    if (!w) return nullptr;
    char* r = nullptr;
    if (d2multi_MultiByteToWideChar(from, 1, s, n, w, wn)) {
        if (out) {
            if (d2multi_WideCharToMultiByte(to, 0, w, wn, out, outLen, nullptr, nullptr)) r = out;
        } else {
            int m = single ? wn : d2multi_WideCharToMultiByte(to, 0, w, wn, nullptr, 0, nullptr, nullptr);
            if (m && (r = (char*)d2multi_heap_alloc(m))) {
                unsigned u = d2multi_WideCharToMultiByte(to, 0, w, m, r, m, nullptr, nullptr);
                if (!u) { free(r); r = nullptr; } else if (n != 0xffffffffu) *cb = u;
            }
        }
    }
    free(w);
    return r;
}
