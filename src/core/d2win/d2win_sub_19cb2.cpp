// src/core/d2win/d2win_sub_19cb2.cpp -- 1.13c D2Win+0x19CB2, MSVC CRT $I10_OUTPUT (80-bit long double -> digits),
// from the cached decompile (build-wasm/ghidra-d2win/decomp/0x19CB2.c).
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" char* d2win_sub_4ed0(char* dst, const char* src);
extern "C" void d2win_sub_1a184(int32_t* x, uint32_t exp10, int32_t noClear, const void* env);  // D2Win+0x1A184
extern "C" void d2win_sub_19f4e(uint8_t* a, const uint8_t* b);                                  // D2Win+0x19F4E
struct D2Win1A184Env { const uint8_t* posTable; const uint8_t* negTable; void (*mul)(void*, const void*); };
// The DLL's power-of-ten tables (D2Win+0x20CC0, negative half at +0x160); set by the host before use.
extern "C" { const uint8_t* g_d2win_tenpow_pos = nullptr; const uint8_t* g_d2win_tenpow_neg = nullptr; }
extern "C" void d2win_scale10(uint32_t* x, int32_t e, int32_t keep) {
    D2Win1A184Env env{g_d2win_tenpow_pos, g_d2win_tenpow_neg, (void (*)(void*, const void*))d2win_sub_19f4e};
    d2win_sub_1a184((int32_t*)x, (uint32_t)e, keep, &env);
}

static void shl12(uint32_t* p) { p[2] = p[2] << 1 | p[1] >> 31; p[1] = p[1] << 1 | p[0] >> 31; p[0] <<= 1; }
static void shr12(uint32_t* p) { p[0] = p[0] >> 1 | p[1] << 31; p[1] = p[1] >> 1 | p[2] << 31; p[2] >>= 1; }
static void add12(uint32_t* a, const uint32_t* b) {
    uint64_t s = (uint64_t)a[0] + b[0]; a[0] = (uint32_t)s;
    s = (uint64_t)a[1] + b[1] + (s >> 32); a[1] = (uint32_t)s;
    a[2] += b[2] + (uint32_t)(s >> 32);
}

// 1.13c D2Win+0x19CB2
// in = x87 extended value (lo, hi, exp/sign word). out: i16 decpt, +2 sign (' ' or '-'), +3 ndigits,
// +4 NUL-terminated digits. param_4 digit count (or decimals if bit0 of mode). Returns 1, or 0 for INF/NAN.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2win_sub_19CB2(uint32_t lo, uint32_t hi, uint16_t se, int32_t ndig, uint8_t mode, int16_t* out) {
    char* o = (char*)out;
    uint32_t e15 = se & 0x7fff;
    uint8_t k[12] = {0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xcc,0xfb,0x3f};
    o[2] = (se & 0x8000) ? '-' : ' ';
    bool zero = false;
    if ((int16_t)e15 == 0 && hi == 0 && lo == 0) zero = true;
    else {
        if ((int16_t)e15 == 0x7fff) {
            out[0] = 1;
            const char* s; int n;
            if ((hi == 0x80000000 && lo == 0) || (hi & 0x40000000)) {
                if (!(se & 0x8000) || hi != 0xc0000000) {
                    if (hi != 0x80000000 || lo != 0) { s = "1#QNAN"; n = 6; } else { s = "1#INF"; n = 5; }
                } else if (lo != 0) { s = "1#QNAN"; n = 6; } else { s = "1#IND"; n = 5; }
            } else { s = "1#SNAN"; n = 6; }
            d2win_sub_4ed0(o + 4, s);
            o[3] = (char)n;
            return 0;
        }
        uint8_t b[12] = {0};
        int16_t dp = (int16_t)(((e15 >> 8) + (hi >> 24) * 2) * 0x4d - 0x134312f4 + e15 * 0x4d10 >> 16);
        b[10] = (uint8_t)e15; b[11] = (uint8_t)(e15 >> 8);
        memcpy(b + 2, &lo, 4); memcpy(b + 6, &hi, 4);
        d2win_scale10((uint32_t*)b, -(int32_t)dp, 1);
        if (0x3ffe < (uint32_t)(b[11] << 8 | b[10])) { dp++; d2win_sub_19f4e((uint8_t*)b, k); }
        out[0] = dp;
        if ((mode & 1) && (ndig += dp, ndig < 1)) zero = true;
        else {
            if (ndig > 0x15) ndig = 0x15;
            int32_t sh = (int32_t)(b[11] << 8 | b[10]) - 0x3ffe;
            b[10] = b[11] = 0;
            uint32_t m[3];
            memcpy(m, b, 12);
            for (int i = 0; i < 8; i++) shl12(m);
            if (sh < 0) for (uint32_t c = -sh & 0xff; c; c--) shr12(m);
            char* d = o + 4; char* p = d;
            for (int32_t n = ndig + 1; n > 0; n--) {
                uint32_t c[3] = {m[0], m[1], m[2]};
                shl12(m); shl12(m); add12(m, c); shl12(m);
                *p++ = (char)((m[2] >> 24) + '0');
                m[2] &= 0x00ffffff;
            }
            char* q = p - 2;   // the last of the ndig + 1 digits only rounds: the strip and carry start one before it
            if (p[-1] < '5') {
                for (; d <= q && *q == '0'; q--) {}
                if (q < d) { *d = '0'; zero = true; }
            } else {
                for (; d <= q && *q == '9'; q--) *q = '0';
                if (q < d) { q++; out[0]++; }
                (*q)++;
            }
            if (!zero) {
                char n = (char)((q - o) - 3);
                o[3] = n; o[4 + n] = 0;
            }
        }
    }
    if (zero) {
        o[4] = '0';
        out[0] = 0; o[2] = ' '; o[3] = 1; o[5] = 0;
    }
    return 1;
}
