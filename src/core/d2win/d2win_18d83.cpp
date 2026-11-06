// src/core/d2win/d2win_18d83.cpp -- D2Win.dll CRT __cftof2 (MSVC 7.1): lays out fixed-point digits in place.
#include <cstdint>
#include <cstring>

// __shift (D2Win+0x18C48): slides the string at p right by n bytes.
static void shift(char* p, int32_t n) { if (n != 0) memmove(p + n, p, strlen(p) + 1); }

// 1.13c D2Win+0x18D83
// __cftof2(buf, nd, flag), EAX = strflt {sign, decpt}: buf holds the __fptostr digits (after a sign slot). Writes
// '-', then either "0" (decpt <= 0) or skips decpt integer digits; with nd > 0 inserts '.' and, for decpt < 0,
// min(nd, -decpt) leading zeros (all -decpt with flag). flag and decpt-1 == nd first truncate with a '0'. Returns buf.
extern "C" char* d2win_cftof2_18d83(char* buf, int32_t nd, int32_t flag, const int32_t* flt) {
    const int32_t neg = flt[0] == '-';
    if (flag && flt[1] - 1 == nd) { char* t = buf + neg + (flt[1] - 1); t[0] = '0'; t[1] = 0; }
    char* p = buf;
    if (neg) *p++ = '-';
    if (flt[1] <= 0) { shift(p, 1); *p++ = '0'; }
    else p += flt[1];
    if (nd > 0) {
        shift(p, 1);
        *p++ = '.';
        if (flt[1] < 0) {
            const int32_t n = -flt[1];
            if (flag || nd >= n) nd = n;
            shift(p, nd);
            memset(p, '0', (size_t)nd);
        }
    }
    return buf;
}
