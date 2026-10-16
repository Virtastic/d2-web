// MSVC CRT string routines statically linked into D2Client.dll.
// Pure on their arguments. The probes take blob pointers as r[0..2] and return in r[4] (checked against retail).
#include <emscripten.h>
#include <cstdint>
#include <cstddef>

// 1.13c D2Client+0x4A60 (_strcmp): -1 / 0 / 1 by unsigned byte compare
extern "C" int32_t d2c_strcmp(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        if (*a != *b) return *a < *b ? -1 : 1;
        if (*a == 0) return 0;
    }
}

// 1.13c D2Client+0x76F0 (___ascii_stricmp): fold A-Z only; result is a signed char (-1/0/1)
static uint8_t d2c_lower(uint8_t c) { return (uint8_t)(c - 'A') < 26 ? (uint8_t)(c + 0x20) : c; }
extern "C" int32_t d2c_stricmp(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        uint8_t x = *a, y = *b;
        if (x != y) {
            x = d2c_lower(x); y = d2c_lower(y);
            if (x != y) return x < y ? -1 : 1;
        }
        if (y == 0) return 0;   // retail tests the terminator on the second string (loop counter byte)
    }
}

// 1.13c D2Client+0x7740 (___ascii_strnicmp): folds the upper-case of either side; stops at either NUL
extern "C" int32_t d2c_strnicmp(const uint8_t* a, const uint8_t* b, uint32_t n) {
    for (; n; --n, ++a, ++b) {
        uint8_t x = *a, y = *b;
        if (x == 0) return y ? -1 : 0;
        if (y == 0) return 1;
        x = d2c_lower(x); y = d2c_lower(y);
        if (x != y) return x < y ? -1 : 1;
    }
    return 0;
}

// 1.13c D2Client+0xB0D0 (_strncmp): compares min(strlen(a)+1, n) bytes, unsigned
extern "C" int32_t d2c_strncmp(const uint8_t* a, const uint8_t* b, uint32_t n) {
    uint32_t len = 0;
    while (len < n && a[len++]) {}
    for (uint32_t i = 0; i < len; ++i)
        if (a[i] != b[i]) return a[i] > b[i] ? 1 : -1;
    return 0;
}

// 1.13c D2Client+0xB110 (_strpbrk): first byte of s that is in set (NUL excluded), else null
extern "C" const uint8_t* d2c_strpbrk(const uint8_t* s, const uint8_t* set) {
    uint8_t m[32] = {};
    for (; *set; ++set) m[*set >> 3] |= (uint8_t)(1u << (*set & 7));
    for (; *s; ++s) if (m[*s >> 3] >> (*s & 7) & 1) return s;
    return nullptr;
}

#define P(i) ((const uint8_t*)(uintptr_t)r[i])
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_4a60(uint32_t* r) { r[4] = (uint32_t)d2c_strcmp(P(0), P(1)); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_76f0(uint32_t* r) { r[4] = (uint32_t)d2c_stricmp(P(0), P(1)); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_7740(uint32_t* r) { r[4] = (uint32_t)d2c_strnicmp(P(0), P(1), r[2]); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_b0d0(uint32_t* r) { r[4] = (uint32_t)d2c_strncmp(P(0), P(1), r[2]); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_b110(uint32_t* r) { r[4] = (uint32_t)(uintptr_t)d2c_strpbrk(P(0), P(1)); }
