#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x1E7E0: screen-layout refresh. Decision logic and the call
// sequence only; the callees are performed by the caller from the returned list.
// st dwords (globals, base 6FAB0000): 0 +1223D8 mode, 1 +1223F0 byte, 2 +1223F3 byte, 3 +FADBC pending, 4 +DD5D0 gate(0x26),
// 5 +FAD84, 6 +FAD90, 7 +11B828 x, 8 +11C418, 9 +11C414, 10 +F9E14 h, 11 +F9E18, 12 +11C3C4, 13 +DBC48 w.
// calls: 7 dwords each {rva, eax, ecx, edx, ebx, esi, stack0}; returns the call count (the result is always 1).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1e7e0(uint32_t arg, uint32_t* st, uint32_t d0b4, uint32_t* calls) {
    uint32_t n = 0;
    auto C = [&](uint32_t rva, uint32_t a, uint32_t c, uint32_t d, uint32_t b, uint32_t s, uint32_t s0) {
        uint32_t* p = calls + n++ * 7; p[0] = rva; p[1] = a; p[2] = c; p[3] = d; p[4] = b; p[5] = s; p[6] = s0;
    };
    if (st[0] == 2) { st[0] = 0; st[1] = 0; st[2] = 0; }
    C(0x1CAD0, 0, 0, 0, 0, 0, 0);
    uint32_t pend = st[3];
    C(0xBF8C, 0, 0, 0, 0, 0, 0);
    // gate != 0x26 -> _exit(-1): not reproducible, the capture pins 0x26
    st[3] = 0;
    if (pend) { C(0xD0B4, 0, 0, 0, 0, 0, 0xB2000000u | 0xDBCC0); C(0x3CC60, 7, 0, 0, 0, 0, d0b4); C(0x1D8F0, 0, 0, 0, 0, 0, 0); C(0xBE2E0, 0, 0, 0, 0, 0, 0); }
    int32_t x = (int32_t)st[7], w = (int32_t)st[13];
    uint32_t h = st[10];
    if (!st[5] && !st[6]) {
        st[8] = 0; st[9] = 0;
        C(0xD1B0, 0, 0, 0, 0, 0, 0);
        C(0x8A4D0, st[11], st[10], 0, 0, st[12], 0);
        if (arg && x > w / 2) { C(0x161B0, 0, 0, 0, (uint32_t)(x - w / 4), 0, 0); }
    } else {
        int32_t q = w / 4;
        st[8] = (uint32_t)-q; st[9] = 1;
        C(0xD1B0, 0, (uint32_t)-q, 0, 0, 0, 0);
        int32_t hq = (int32_t)h / 4;
        C(0x8A4D0, st[11], h - (uint32_t)hq, 0, 0, st[12], (uint32_t)-hq);
    }
    return n;
}
