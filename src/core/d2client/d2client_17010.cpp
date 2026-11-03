// 1.13c D2Client+0x17010 -- ai spawn: with the player slot (D2Client+0x11BBFC) empty, set the unit's mode field to 5 and return its
// +0x30 block; otherwise one LCG step on the unit seed (+0x20/+0x24): a roll of 5 or more in 100 returns the quotient. Else a second step
// picks the kind 0x23A + (lo & 3), the unit's position (D2Common 0xC3BE/0xC2EC for non-static types) is jittered by two further rolls mod 60
// (-30..29), D2Client 0xAFF10 spawns the object (EDX = unit, ECX = kind, 5 stack words), a fifth roll's low bit picks the launch direction
// (0xC7E30 with 300 or -100), the object's motion block gets the velocity words and flag 0x20, then 0xC7FB0 and the four D2Common setters.
#include "cl_env.h"
extern "C" uint32_t d2c_17010(uint32_t u, const cl::Env& e) {
    if (!e.self) {
        if (!u || e.R(u) != 1) return 0;
        uint32_t o = e.R(u + 0x14);
        if (!o) return 0;
        e.W(o + 0x30, 5);
        return o + 0x30;
    }
    uint32_t s = e.Lcg(u + 0x20);
    if (s % 100 > 4) return s / 100;
    uint32_t kind = (e.Lcg(u + 0x20) & 3) + 0x23a;
    int32_t px = (int32_t)e.PD(u, 0xc3be, 0xc);
    uint32_t r1 = e.Lcg(u + 0x20);
    int32_t py = (int32_t)e.PD(u, 0xc2ec, 0x10);
    uint32_t r2 = e.Lcg(u + 0x20);
    uint32_t o = e.H(0xaff10, kind, (uint32_t)((int32_t)(r1 % 0x3c) - 0x1e + px), (uint32_t)((int32_t)(r2 % 0x3c) - 0x1e + py), 0, 1, 0, kind, u);
    if (!o) return 0;
    bool even = (e.Lcg(u + 0x20) & 1) == 0;
    e.H(0xc7e30, 0, 0, 0, 0, 0, 0, even ? 300u : (uint32_t)-100, 0);
    for (int i = 0; i < 2; i++) {
        uint32_t pa = e.R(o + 0x54), d = pa ? e.R(pa + 0x30) : 0;
        if (!d) continue;
        if (i == 0) { e.W(d + 0x10, 0); e.W(d + 0x14, 0); e.W(d + 0x18, even ? 0xffffd000u : 0x3000u); }
        else        { e.W(d + 0x28, 0); e.W(d + 0x2c, 0); e.W(d + 0x30, even ? 0xfffce000u : 0x96000u); }
    }
    { uint32_t pa = e.R(o + 0x54), d = pa ? e.R(pa + 0x30) : 0; if (d) { uint32_t f = e.R(d); e.W(d, even ? (f & 0xffffffdfu) : (f | 0x20)); } }
    e.H(0xc7fb0, o);
    e.H(0xc4a2, o, 0x3c);
    e.H(0xc2c8, o, 0x3c);
    e.H(0xc448, o, 100);
    return e.H(0xc394, o, 300);
}
