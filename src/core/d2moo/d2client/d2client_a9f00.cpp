// 1.13c D2Client+0xA9F00 (no arguments): rebuild the 9-entry id list of the current level.
// g = the module bytes 0xFCD8C.. (0x56): 9 packed entries {int32 id, uint8 flag} at +0 (45 bytes), dword 0xFCDD1 (+0x45, unaligned) = C57A's first argument,
// byte 0xFCDD5 (+0x49), dword 0xFCDD6 (+0x4A) = level, dword 0xFCDDA (+0x4E) = entry count, dword 0xFCDDE (+0x52) = count of entries whose id differs from the unit's.
// rt = the 6-entry range table at 0xDE698 (11 bytes each: flag, int32 lo, int32 hi, byte min, byte max); unitH = DAT 0x11BBFC.
// level above 5: byte 0xFCDD5 = 0 and done. Range flag 0: set it and scan i = lo..hi with C580(i, &out): a hit sets min = out (when still 0xFF) and max = max(max, out).
// Then v = min..max: C57A(DAT 0xFCDD1, v) nonzero: a C598(v, &t) hit stores entry id t, flag = (t != C1DE(C1F0(unit))), count++; C57A zero: a C598 hit stores entry id t, flag 0.
// C580 and C598 write their out slots as locals of the caller; the probe's hookw writes a dword there.
#include <cstring>
#include <emscripten.h>
#include "d2client_a9_env.h"
static uint32_t r32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static void w32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_a9f00(uint8_t* g, uint8_t* rt, uint32_t unitH, const D2cA9Env* e) {
    uint32_t A = 0, B = 0;
    w32(g + 0x4e, 0); w32(g + 0x52, 0);
    const uint32_t u = e->call(e->ctx, 0xC1F0, 0, 0, 0, 0, unitH, 0, 0, 0, 0);
    const uint32_t ebp = e->call(e->ctx, 0xC1DE, 0, 0, 0, 0, u, 0, 0, 0, 0);
    for (int k = 0; k < 9; ++k) { w32(g + 5 * k, 0); g[5 * k + 4] = 0; }
    uint32_t lvl = r32(g + 0x4a);
    if (lvl > 5) { g[0x49] = 0; return; }
    uint8_t* R = rt + lvl * 11;
    if (R[0] == 0) {
        R[0] = 1;
        const int32_t lo = (int32_t)r32(R + 1), hi = (int32_t)r32(R + 5);
        for (int32_t i = lo; i <= hi;) {
            const uint32_t rv = e->call(e->ctx, 0xC580, 0, 0, 0, 0, (uint32_t)i, d2c_hp(&A), 0, 0, 0);
            lvl = r32(g + 0x4a);
            uint8_t* Q = rt + lvl * 11;
            if (rv) {
                const uint8_t dl = (uint8_t)A;
                if (Q[9] == 0xff) Q[9] = dl;
                uint8_t cl = Q[10];
                if (dl > cl) cl = dl;
                Q[10] = cl;
            }
            ++i;
            if (i > (int32_t)r32(rt + lvl * 11 + 5)) break;
        }
    }
    R = rt + lvl * 11;
    int32_t v = R[9];
    if (v > (int32_t)R[10]) return;
    for (;;) {
        if (e->call(e->ctx, 0xC57A, 0, 0, 0, 0, r32(g + 0x45), (uint32_t)v, 0, 0, 0)) {
            if (e->call(e->ctx, 0xC598, 0, 0, 0, 0, (uint32_t)v, d2c_hp(&A), 0, 0, 0)) {
                const uint32_t idx = r32(g + 0x4e);
                w32(g + 5 * idx, A);
                if (ebp == A) g[5 * idx + 4] = 0;
                else { const uint32_t c = r32(g + 0x52) + 1; g[5 * idx + 4] = 1; w32(g + 0x52, c); }
                w32(g + 0x4e, idx + 1);
            }
        } else if (e->call(e->ctx, 0xC598, 0, 0, 0, 0, (uint32_t)v, d2c_hp(&B), 0, 0, 0)) {
            const uint32_t idx = r32(g + 0x4e);
            w32(g + 5 * idx, B);
            g[5 * idx + 4] = 0;
            w32(g + 0x4e, idx + 1);
        }
        ++v;
        if (v > (int32_t)rt[r32(g + 0x4a) * 11 + 10]) break;
    }
}
