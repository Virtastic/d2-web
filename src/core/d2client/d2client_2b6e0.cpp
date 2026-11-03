// 1.13c D2Client+0x2B6E0 -- skill start, the owner-state variant: ECX = unit, [esp+4] unused, ret 4.
// owner = 0xC310(unit) (0: return 0); F = 0xC628(owner); ebp = 15 (0xCEF8(unit) when 0xCD9C(unit) is set); L = 7, or -1 when the first byte at
// +5 of the 0xCD9C record is 0 (the 0x2B4F0 helper, inlined). A unit of type 0/1 whose +0x4E byte is 3 only calls 0x242B0 and returns 0.
// Then F bit 0 / bit 1 pick the three outcomes recovered from the decompile.
#include "cl_mb2.h"
extern "C" uint32_t d2c_2b680(uint32_t, uint32_t, const clm::Env&);
extern "C" uint32_t d2c_2b6e0(uint32_t u, const clm::Env& e) {
    uint32_t owner = e.H(0xc310, 0, 0, 0, 0, 0, u);
    if (!owner) return 0;
    uint32_t F = e.H(0xc628, 0, 0, 0, 0, 0, owner);
    uint32_t ebp = 0xf;
    if (e.H(0xcd9c, 0, 0, 0, 0, 0, u)) ebp = e.H(0xcef8, 0, 0, 0, 0, 0, u);
    int32_t L = 7;
    uint32_t rec = e.H(0xcd9c, 0, 0, 0, 0, 0, u);
    if (rec) {
        int32_t cnt = (int32_t)e.H(0xcef8, 0, 0, 0, 0, 0, u);
        if (cnt > 0 && e.R8(rec + 5) == 0) L = -1;
    }
    if (u && (e.R(u) == 0 || e.R(u) == 1) && e.R8(u + 0x4e) == 3) {
        e.H(0x242b0, 0, u, 0, 0, u, e.R(u + 0x10));
        return 0;
    }
    uint32_t G = e.H(0xcb68, 0, 0, 0, 0, 0, owner);
    uint32_t Hh = e.H(0xcb6e, 0, 0, 0, 0, 0, owner);
    uint32_t ebx = 0;
    if (G != 6) ebx = e.H(0xa4e20, Hh & 0x7f, Hh, clm::Env::Mod(0x10a608 + (G << 9)), 0, 0, G);
    if (F & 1) {
        if (ebx && e.H(0xc6e2, 0, 0, 0, 0, 0, u, ebx, 0)) {
            e.H(0xc60a, 0, 0, 0, 0, 0, owner, 0);
            e.H(0xcb56, 0, 0, 0, 0, 0, u, (uint32_t)(L + 1));
            e.W(u + 0x48, ebp << 8);
            return 1;
        }
        if (F & 2) goto bit2;
        goto bit2clear;
    }
    // F bit 0 clear
    e.H(0xc60a, 0, 0, 0, 0, 0, owner, 0);
    e.H(0xcb50, 0, 0, 0, 0, 0, owner, 0);
    e.H(0xcb7a, 0, 0, 0, 0, 0, owner, 0);
    e.W(u + 0x48, (ebp - ((int32_t)e.R(u + 0x38) >> 8)) << 8);
    if (!ebx) {
        ebx = d2c_2b680(0xffffffffu, u, e);
        if (!ebx) return 0;
    }
    {
        uint32_t v60 = e.H(0x1660, 0, ebx);
        uint32_t v30 = e.H(0x1630, 0, ebx);
        e.H(0xc478, 0, 0, 0, 0, 0, e.R(u + 0x2c), v30, v60, 0);
        return 1;
    }
bit2:
    ebx = d2c_2b680(Hh, u, e);
    if (ebx && e.H(0xc6e2, 0, 0, 0, 0, 0, u, ebx, 0)) {
        e.H(0xc60a, 0, 0, 0, 0, 0, owner, 0);
        e.H(0xcb50, 0, 0, 0, 0, 0, owner, e.R(ebx));
        e.H(0xcb7a, 0, 0, 0, 0, 0, owner, e.R(ebx + 0xc));
        return 1;
    }
    e.H(0xc60a, 0, 0, 0, 0, 0, owner, 0);
    if (u && e.R(u) == 1) {
        uint32_t a = e.H(0x1700, u);
        a = e.H(0x1740, a);
        a -= 0x1b4;
        if (a == 0) return 1;
        a -= 5;
        if (a == 0) return 1;
    }
    e.W(u + 0x48, 0x100);
    return 1;
bit2clear:
    if (((int32_t)e.R(u + 0x38) >> 8) >= L) {
        e.H(0xcb56, 0, 0, 0, 0, 0, u, 0);
        e.W(u + 0x48, ebp << 8);
        e.H(0xc60a, 0, 0, 0, 0, 0, owner, 1);
    }
    e.W(u + 0xc4, e.R(u + 0xc4) & ~0x40u);
    {
        int32_t a = (int32_t)e.H(0xcb9e, 0, 0, 0, 0, 0, owner);
        if (a < 2) a++;
        e.H(0xcb8c, 0, 0, 0, 0, 0, owner, (uint32_t)a);
        return 1;
    }
}
