// 1.13c D2Win+0x1935A (CRT _atoflt): parse a string to a 4-byte float. Runs the string parser (0x19872, result in a
// 12-byte LDBL12) and narrows it to a float (0x192FF, __ld12tof folded with __ld12tod by the linker) into *out.
// LDBL12 layout: bytes 0-1 guard bits, 2-9 mantissa with explicit integer bit, 10-11 sign and exponent (bias 0x3FFF).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void d2win_sub_19872(void* ld12, int32_t* consumed, const uint8_t* s, int, int, int, int);

namespace {
// round-to-nearest-even narrowing of the 80-bit value (plus the guard word) to IEEE single
uint32_t ld12tof(const uint8_t* ld) {
    uint16_t g; uint64_t m; uint16_t se;
    memcpy(&g, ld, 2); memcpy(&m, ld + 2, 8); memcpy(&se, ld + 10, 2);
    uint32_t sign = (se & 0x8000u) ? 0x80000000u : 0u;
    int32_t e = (int32_t)(se & 0x7FFF) - 0x3FFF;
    if ((se & 0x7FFF) == 0 || m == 0) return sign;
    if ((se & 0x7FFF) == 0x7FFF) return sign | 0x7F800000u;
    int32_t be = e + 127;                       // biased single exponent
    int shift = 40;                             // 64-bit mantissa to 24 bits
    if (be <= 0) shift += 1 - be;               // denormal: shift further
    if (shift > 64) return sign;
    uint64_t q = shift == 64 ? 0 : m >> shift;
    uint64_t rem = shift == 64 ? m : m & ((1ull << shift) - 1);
    uint64_t half = 1ull << (shift - 1);
    bool sticky = g != 0 || (rem & (half - 1)) != 0;
    bool up = (rem & half) && (sticky || (q & 1));
    if (up) q++;
    if (be <= 0) return sign | (uint32_t)q;     // q may carry into the smallest normal
    if (q >> 24) { q >>= 1; be++; }
    if (be >= 255) return sign | 0x7F800000u;
    return sign | ((uint32_t)be << 23) | ((uint32_t)q & 0x7FFFFFu);
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2win_sub_1935a(void* out, const uint8_t* str) {
    uint8_t ld12[12];
    int32_t consumed;
    d2win_sub_19872(ld12, &consumed, str, 0, 0, 0, 0);
    uint32_t f = ld12tof(ld12);
    memcpy(out, &f, 4);
}
