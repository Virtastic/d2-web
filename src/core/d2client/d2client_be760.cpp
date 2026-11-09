// 1.13c D2Client+0xBE760 stripSubstringRepeated (stdcall, one stack word s, returns s): D2Lang #10003(0xF9A) gives the needle; every match that is not the last thing in s is removed together with the one char after it.
// Unicode::strlen / ::strstr are pure D2Lang imports (native here); the memmove callee 0x31A0 is memmove. Over 499 chars: Fog #10265(0x37F), Fog #10024(msg, id), exit(-1).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "client_host.h"
static uint32_t wlen(const uint16_t* s) { uint32_t n = 0; while (s[n]) n++; return n; }
static uint16_t* wstr(uint16_t* s, const uint16_t* p) {
    for (; ; s++) { uint32_t i = 0; while (p[i] && s[i] == p[i]) i++; if (!p[i]) return s; if (!*s) return nullptr; }
}
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_be760(D2cHost h, uint16_t* s) {
    if (wlen(s) >= 500) {
        D2cCall c = {}; c.rva = 0xBF92; c.s[0] = 0x37f; uint32_t t = h(&c);
        c = {}; c.rva = 0xBF80; c.s[0] = 0x6fb8359f; c.s[1] = t; h(&c);
        c = {}; c.rva = 0x3664; c.s[0] = 0xffffffffu; h(&c);
    }
    D2cCall c = {}; c.rva = 0xD1EC; c.ecx = 0xf9a; const uint16_t* pat = (const uint16_t*)(uintptr_t)h(&c);
    uint32_t n = wlen(pat);
    uint16_t* d = wstr(s, pat);
    while (d && d[n] != 0) {
        memmove(d, d + n + 1, (wlen(d) - n) * 2);
        d = wstr(s, pat);
    }
    return (uint32_t)(uintptr_t)s;
}
