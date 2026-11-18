// 1.13c D2Client+0x181A0 -- scatter-spawn one object of monster class `cls` into room `room`: ESI = the 64-bit LCG seed (lo, hi),
// stdcall(room, cls), ret 8. gl+0 = the spawn counter 0xDE834. Returns nothing (the capture's result word is 0).
// Bounds {x, y, w, h} come from 0xC5B0(room, &buf); the class row (dt+0xA78 stride 0x1A8, +0x18 = a short index into the table
// at dt+0xA90 stride 0x134) gives the collision-size byte at +8. Up to 10 tries: x' = x + rnd(w-1), y' = y + rnd(h-1), accepted
// when 0xC34C(room, x', y', size, 0x3F11) returns 0 in AX; the object is then spawned through 0xA7780(&rec, 0, 0, 0, 0, 1)
// with rec = {++counter (wraps -1 to 0), cls, x', y', 0, 0, 1}.
#include "cl_env.h"
static uint32_t rnd181a0(const cl::Env& e, uint32_t sp, int32_t lim) {
    if (lim <= 0) return 0;
    uint64_t s = (uint64_t)e.R(sp) * 0x6ac690c5u + e.R(sp + 4);
    e.W(sp + 4, (uint32_t)(s >> 32));
    e.W(sp, (uint32_t)s);
    uint32_t l = (uint32_t)lim;
    return (l & (l - 1)) ? (uint32_t)s % l : (l - 1) & (uint32_t)s;
}
extern "C" uint32_t d2c_181a0(uint32_t seed, uint32_t room, uint32_t cls, const cl::Env& e) {
    uint32_t buf[4] = {0, 0, 0, 0};
    e.H(0xc5b0, room, (uint32_t)(uintptr_t)buf);
    uint32_t x = buf[0], y = buf[1];
    int32_t lw = (int32_t)buf[2] - 1, lh = (int32_t)buf[3] - 1;
    if ((int32_t)cls < 0 || (int32_t)cls >= (int32_t)e.R(e.dt + 0xa80)) return 0;
    int32_t idx = e.R16s(e.R(e.dt + 0xa78) + cls * 0x1a8u + 0x18);
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xa98)) return 0;
    uint32_t rec2 = (uint32_t)idx * 0x134u + e.R(e.dt + 0xa90);
    if (!rec2) return 0;
    uint32_t cd = (uint32_t)(int32_t)(int8_t)e.R8(rec2 + 8), px = 0, py = 0;
    int32_t i = 0;
    for (;;) {
        px = x + rnd181a0(e, seed, lw);
        py = y + rnd181a0(e, seed, lh);
        if (!(e.H(0xc34c, room, px, py, cd, 0x3f11) & 0xffff)) break;
        if (++i >= 10) return 0;
    }
    uint32_t c = e.R(e.gl) + 1;
    if (c == 0xffffffffu) c = 0;
    e.W(e.gl, c);
    uint32_t rec[7] = {c, cls, px, py, 0, 0, 1};
    uint32_t p = (uint32_t)(uintptr_t)rec;
    e.H(0xa7780, p, 0, 0, 0, 0);
    if (e.blk) e.blk(e.ctx, 0xa7780, p, 7);
    return 0;
}
