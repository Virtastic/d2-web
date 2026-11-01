// Shared by the D2Client text formatters (0x2C650, 0x2C790, 0x2C7E0): wsprintfA("%d"-style) into a 32-byte stack
// buffer, then Unicode::toUnicode(dst, buf, 0x20): the ASCII text is widened up to and including its NUL.
#pragma once
#include <cstdint>
namespace d2client_wfmt {
struct Buf { char b[32]; int n = 0; };
inline void put(Buf& o, char c) { if (o.n < 31) o.b[o.n] = c; ++o.n; }
inline void putInt(Buf& o, int32_t v) {
    char t[12]; int k = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[k++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) put(o, '-');
    while (k) put(o, t[--k]);
}
inline void widen(const Buf& o, uint16_t* dst) {
    int n = o.n > 31 ? 31 : o.n;
    for (int i = 0; i < n; ++i) dst[i] = (uint8_t)o.b[i];
    dst[n] = 0;
}
}
