// D2Lang's Unicode::toUnicode (1.13c D2Lang+0x8CD0, fastcall dst, src, n; ret 4) and its UTF-8 sequence decoder (D2Lang+0x8C40, table D2Lang+0x102E8),
// for the D2Client ports that widen a narrow name into a stack buffer. At most n-1 characters, then a NUL; a bad sequence ends the copy one slot later.
#pragma once
#include <cstdint>
namespace d2c {
struct U8Row { uint16_t mask, val; uint32_t shift, keep, min; };
static const U8Row kU8[6] = {
    {0x80, 0x00, 0, 0x7f, 0}, {0xe0, 0xc0, 6, 0x7ff, 0x80}, {0xf0, 0xe0, 12, 0xffff, 0x800},
    {0xf8, 0xf0, 18, 0x1fffff, 0x10000}, {0xfc, 0xf8, 24, 0x3ffffff, 0x200000}, {0xfe, 0xfc, 30, 0x7fffffff, 0x4000000}};
// D2Lang+0x8C40: EAX = bytes left, ECX = sequence, stack = out; returns the sequence length or -1 (out untouched)
inline int32_t u8dec(const uint8_t* p, int32_t left, uint16_t* out) {
    if (left <= 0) return -1;
    const uint16_t b = p[0]; int32_t v = (int16_t)b, n = 0;
    for (int r = 0; r < 6; ++r) {
        ++n;
        if ((kU8[r].mask & b) == kU8[r].val) {
            v &= (int32_t)kU8[r].keep;
            if (v < (int32_t)kU8[r].min) return -1;
            *out = (uint16_t)v; return n;
        }
        if (left <= n) return -1;
        const uint8_t c = (uint8_t)(*++p ^ 0x80);
        if (c & 0xc0) return -1;
        v = v << 6 | c;
    }
    return -1;
}
inline uint16_t* toUnicode(uint16_t* dst, const char* src, int32_t n) {
    const uint8_t* s = (const uint8_t*)src; int32_t len = 0; while (s[len]) ++len;
    int32_t k = 0, i = 0;
    if (s[0]) {
        for (;;) {
            if (k >= n - 1) break;
            const int32_t r = u8dec(s + i, len - i, dst + k); ++k;
            if (r == -1) break;
            i += r;
            if (!s[i]) break;
        }
    }
    dst[k] = 0; return dst;
}
// Unicode::unicodenwidth (D2Lang+0x8D70, fastcall s, n): the characters in the first n bytes; a bad sequence skips one byte uncounted
inline uint32_t unicodenwidth(const char* s, int32_t n) {
    const uint8_t* p = (const uint8_t*)s; uint32_t c = 0; uint16_t t;
    while (n > 0 && *p) {
        const int32_t r = u8dec(p, n, &t);
        if (r == -1) { ++p; --n; continue; }
        for (int32_t i = 0; i < r; ++i) if (!*p++) return c;
        ++c; n -= r;
    }
    return c;
}
inline void wcat16(uint16_t* d, const uint16_t* s) { while (*d) ++d; do { *d++ = *s; } while (*s++); }
// Unicode::strncpy (D2Lang+0xB0D0): at most n chars, stop at the NUL, zero-fill the rest of the n (no NUL when n chars were copied)
inline void wncpy16(uint16_t* d, const uint16_t* s, int32_t n) { int32_t i = 0; for (; i < n && s[i]; ++i) d[i] = s[i]; for (; i < n; ++i) d[i] = 0; }
// Unicode::win2Unicode (D2Lang+0x8320): widen at most n bytes, stop at the NUL; a source cut short at n ends one char early
inline void win2Unicode(uint16_t* d, const char* s, int32_t n) {
    int32_t i = 0; for (; i < n && s[i]; ++i) d[i] = (uint8_t)s[i];
    if (s[i]) d[i - 1] = 0; else d[i] = 0;
}
}  // namespace d2c
