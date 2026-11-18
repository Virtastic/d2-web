// 1.13c D2Client+0xb6120 -- client missile: one child at the unit's position; ECX = missile unit, always returns 1.
// Skill index = D2Common 0xC388(u) must index the 0x23C-stride table (dt +0xB98 / +0xBA0) or nothing spawns.
// Duration = Missiles row +0x6C, or D2Common 0xC316(room, skill +0x138, skill, level) clamped to >= 5 when it is 0;
// a positive duration makes the request 0x8001 with the duration in block word 19. The child then gets
// D2Common 0xC3C4(child path, 0xC2D4(unit path)).
#include "cl_env.h"
extern "C" uint32_t d2c_b6120(uint32_t u, const cl::Env& e) {
    if (!u) return 1;
    int32_t cid = (int32_t)e.R(u + 4);
    if (cid < 0) return 1;
    uint32_t row = e.Row(cid);
    if (!row) return 1;
    if (e.R16s(row + 0x2c) < 0) return 1;
    uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    if (!room) return 1;
    int32_t sk = (int32_t)e.H(0xc388, u);
    uint32_t lv = e.H(0xc3fa, u);
    if (sk < 0 || sk >= (int32_t)e.R(e.dt + 0xba0)) return 1;
    uint32_t srow = e.R(e.dt + 0xb98) + (uint32_t)sk * 0x23c;
    if (!srow) return 1;
    int32_t dur = (int32_t)e.R(row + 0x6c);
    if (dur == 0) {
        dur = (int32_t)e.H(0xc316, room, e.R(srow + 0x138), (uint32_t)sk, lv);
        if (dur < 5) dur = 5;
    }
    uint32_t q[23] = {};
    q[0] = 1; q[1] = room;
    q[4] = (uint32_t)e.R16s(row + 0x2c);
    q[11] = lv; q[12] = lv;
    q[5] = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
    q[6] = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
    if (dur > 0) { q[0] = 0x8001; q[19] = (uint32_t)dur; }
    uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
    if (m) {
        uint32_t t = e.H(0xc2d4, e.R(u + 0x2c));
        e.H(0xc3c4, e.R(m + 0x2c), t);
    }
    return 1;
}
