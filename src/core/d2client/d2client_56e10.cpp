// 1.13c D2Client+0x56E10 -- set a unit's attack-rate frame values (EAX = skill, EBX = unit; stack p1, p2; ret 8). Skill out of
// the skill-desc table (+0xB98 / +0xBA0, stride 0x23C), no unit, or its class out of the 0x1A4 table (+0xB64 / +0xB6C): 0.
// v = 0xC316 (p1, skrow+0x138, skill, p2); v <= 0: v = classrow+0x3C - 1 + p2; clamp to 1..0xFF. 0xC3B8 (unit+0x2C, v),
// 0xC2FE (unit, (byte classrow+0x178 << 8) / v), 0xC4A2 (unit, v), 0xC2C8 (unit, v); 1.
#include "cl_env.h"
extern "C" uint32_t d2c_56e10(uint32_t skill, uint32_t unit, uint32_t p1, uint32_t p2, const cl::Env& e) {
    if ((int32_t)skill < 0 || (int32_t)skill >= (int32_t)e.R(e.dt + 0xba0)) return 0;
    uint32_t sk = e.R(e.dt + 0xb98) + skill * 0x23c;
    if (!sk || !unit) return 0;
    uint32_t mr = e.UnitRow(unit);
    if (!mr) return 0;
    int32_t v = (int32_t)e.H(0xc316, p1, e.R(sk + 0x138), skill, p2);
    if (v <= 0) v = (int32_t)(e.R(mr + 0x3c) + p2 - 1);
    if (v <= 1) v = 1; else if (v >= 0xff) v = 0xff;
    e.H(0xc3b8, e.R(unit + 0x2c), (uint32_t)v);
    e.H(0xc2fe, unit, (uint32_t)((int32_t)(e.R8(mr + 0x178) << 8) / v));
    e.H(0xc4a2, unit, (uint32_t)v);
    e.H(0xc2c8, unit, (uint32_t)v);
    return 1;
}
