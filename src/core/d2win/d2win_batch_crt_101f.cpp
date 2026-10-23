// src/core/d2win/d2win_batch_crt_101f.cpp -- VC7.1 CRT leaves of D2Win.dll (statically linked msvcrt
// code), from the cached decompiles. Alpha batch lane: implementation + marker, no spec/vector.
// Not ported (process/CRT lifecycle with no wasm counterpart, or callees unported): 0x1077, 0x10D1,
// 0x113B, 0x16FD, 0x187E, 0x1962, 0x1995, 0x1AAE.
#include <cstdint>
#include <cstdarg>
#include <cstdio>
#include <emscripten.h>

// 1.13c D2Win+0x0101f (_sprintf): format into the caller's buffer through the CRT formatter, NUL-terminate,
// return the character count (excluding the NUL). A null buffer still formats and counts (the string
// stream has 0x7fffffff room; the terminator is skipped when _Dest is null).
extern "C"
EMSCRIPTEN_KEEPALIVE int d2win_sub_101F(char* dest, const char* fmt, ...) {
    va_list ap; va_start(ap, fmt);
    int n = vsnprintf(dest, dest ? 0x7fffffff : 0, fmt, ap);
    va_end(ap);
    return n;
}

// 1.13c D2Win+0x01570 (_strncpy): copy up to count bytes of src, then zero-fill the rest of count;
// no terminator when src is count or longer. Returns dest. count==0 touches nothing.
extern "C"
EMSCRIPTEN_KEEPALIVE char* d2win_sub_1570(char* dest, const char* src, uint32_t count) {
    uint32_t i = 0;
    for (; i < count && src[i]; i++) dest[i] = src[i];
    for (; i < count; i++) dest[i] = 0;
    return dest;
}

// 1.13c D2Win+0x1694 (_stricmp): case-insensitive compare. Retail's locale branch (non-"C" locale
// via _tolower_mt) is not modelled; this is the ___ascii_stricmp path the "C" locale takes: fold A-Z,
// stop at the first difference or NUL, return the signed difference of the folded bytes.
extern "C"
EMSCRIPTEN_KEEPALIVE int d2win_sub_1694(const char* a, const char* b) {
    int x, y;
    do {
        x = (uint8_t)*a++; y = (uint8_t)*b++;
        if (x >= 'A' && x <= 'Z') x += 32;
        if (y >= 'A' && y <= 'Z') y += 32;
    } while (x && x == y);
    return x - y;
}
