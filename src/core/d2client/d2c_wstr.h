// Wide-string helpers for the D2Client item-description ports (D2Lang's Unicode::strcpy / strcat / toUnicode on 16-bit chars, addresses as uint32).
#pragma once
#include <cstdint>
namespace d2c {
inline uint16_t* wp(uint32_t a) { return (uint16_t*)(uintptr_t)a; }
inline void wcat(uint32_t dst, uint32_t src) { uint16_t* d = wp(dst); const uint16_t* s = wp(src); while (*d) ++d; do { *d++ = *s; } while (*s++); }
inline void wcpy(uint32_t dst, uint32_t src) { uint16_t* d = wp(dst); const uint16_t* s = wp(src); do { *d++ = *s; } while (*s++); }
// "%d" into a 10-byte buffer (snprintf semantics, at most 9 characters) widened by toUnicode(dst, src, 9)
inline void wdec(uint16_t* out, int32_t v) {
    char t[16]; int n = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v; char r[12]; int m = 0;
    do { r[m++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) t[n++] = '-';
    while (m) t[n++] = r[--m];
    if (n > 9) n = 9;
    for (int i = 0; i < n; ++i) out[i] = (uint16_t)(uint8_t)t[i];
    out[n] = 0;
}
}  // namespace d2c
namespace d2c {
// Unicode::sprintf(256, dst, fmt, ...) for the formats the item-stat descriptions use: literal text, "%d" (signed 32-bit) and "%%"; one 256-char destination.
inline void wfmt(uint16_t* dst, uint32_t fmt, const int32_t* v, int nv) {
    const uint16_t* f = wp(fmt); uint32_t o = 0; int k = 0;
    while (*f && o < 255) {
        if (*f == '%' && f[1] == 'd') { uint16_t t[16]; int32_t x = k < nv ? v[k] : 0; ++k; uint32_t u = x < 0 ? 0u - (uint32_t)x : (uint32_t)x; uint16_t r[12]; int m = 0;
            do { r[m++] = (uint16_t)('0' + u % 10); u /= 10; } while (u);
            int n = 0; if (x < 0) t[n++] = '-'; while (m) t[n++] = r[--m];
            for (int i = 0; i < n && o < 255; ++i) dst[o++] = t[i]; f += 2; }
        else if (*f == '%' && f[1] == '%') { dst[o++] = '%'; f += 2; }
        else dst[o++] = *f++;
    }
    dst[o] = 0;
}
}  // namespace d2c
