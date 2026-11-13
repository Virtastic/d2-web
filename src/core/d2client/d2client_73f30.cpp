// 1.13c D2Client+0x73F30 -- skill range test: is the target unit within the skill description's range (squared level-scaled
// value at desc+0x64) of the caster, and may the caster act on it. ECX = unit, EDX = skill description index, stack = level
// (ret 4). Returns 0 / 1.
#include "cl_env.h"
extern "C" uint32_t d2c_73f30(uint32_t unit, int32_t idx, uint32_t level, const cl::Env& e) {
    if (idx < 0) return 0;
    const uint32_t dt = e.dt;
    if (idx >= (int32_t)e.R(dt + 0xba0)) return 0;
    const uint32_t desc = (uint32_t)idx * 0x23c + e.R(dt + 0xb98);
    if (!desc) return 0;
    e.H(0x2a0b0, 0, 0, 0, 0, 0, unit);
    uint32_t eax = e.R(unit + 0x2c);
    if (!eax) {
        const uint32_t v = e.H(0xbf92, 0x946);
        e.H(0xbf80, v, 0x6FB8359Fu);
        eax = e.H(0x3664, 0xffffffffu);
    }
    const uint32_t tgt = e.H(0xc2da, eax);
    if (!tgt) return 0;
    int32_t sq = (int32_t)e.H(0xc316, unit, e.R(desc + 0x64), (uint32_t)idx, level);
    sq = (int32_t)((uint32_t)sq * (uint32_t)sq);
    const uint32_t tx = e.PD(tgt, 0xc2ec, 0x10), ty = e.PD(tgt, 0xc3be, 0xc);
    const uint32_t ox = e.PD(unit, 0xc2ec, 0x10), oy = e.PD(unit, 0xc3be, 0xc);
    const int32_t d = (int32_t)e.H(0xc6fa, oy, ox, ty, tx);
    if (d > sq) return 0;
    const int32_t type = (int32_t)e.R(tgt);
    if (type < 0 || type > 1) return 1;
    uint32_t h = e.H(0xc1f0, unit);
    if (!h || e.H(0xc208, h)) return 0;
    h = e.H(0xc1f0, tgt);
    if (!h || e.H(0xc208, h)) return 0;
    return e.H(0xa62a0, unit, 0, 0, 0, 0, tgt) ? 1 : 0;
}
