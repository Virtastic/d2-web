// 1.13c D2Game+0xA2100: A2100 skill-granted-item action.
// code (stack arg0) == 5 returns. flag (edx) == 0: A68E(unit, 0x55) nonzero then A3CA(unit, 0x55, 0). ed = A3D0(unit, code; arg1) (D2Common) zero returns. flag != 0: ADF0(unit, code, arg1); else local = A3B2(unit) and AE08(unit, code, arg1). l2 = -1; AE8C(ed, &l2, 0, 0, 0) (l2 may be written). If unit type 0: AB710(al = type, edx = unit[3]; v = A4C0(unit)[0x9C], flag, code, l2). flag != 0 returns. local != 0: sk = A49C(local, "Skills.cpp", 0xDB3); row = 1420(sk); row and bm[5] & row[4]: s = short row+0x80; (s >= 0 or s < dt[0xC4]) then it = A5D4(unit, s), on it ABB6(unit, it) and ABAA(it), then A3CA(unit, s, 0); then DF780(unit) and each unit+0xDC list node (next at +0x24) with type 8 and [+0x14] == -1 gets 8E6F0. Finally code in [0, dt[0xBA0]) and bm[5] & dtrow[4]: A1D10(edi = unit; DF780(unit), code).
#include "d2game_seam4e.h"
extern "C" void d2g_a2100(uint32_t unit, uint32_t flag, int32_t code, uint32_t arg1, uint32_t dt, uint32_t bm, const D2Seam* e)
{
    if (code == 5) return;
    if (!flag && e->call(0xA68E, 0, 0, 0, 0, 0, unit, 0x55)) e->call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x55, flag);
    const uint32_t ed = e->call(0xA3D0, 0, unit, (uint32_t)code, 0, 0, arg1);
    if (!ed) return;
    uint32_t local = 0;
    if (flag) e->call(0xADF0, 0, 0, 0, 0, 0, unit, (uint32_t)code, arg1);
    else {
        local = e->call(0xA3B2, 0, 0, 0, 0, 0, unit);
        e->call(0xAE08, 0, 0, 0, 0, 0, unit, (uint32_t)code, arg1);
    }
    uint32_t l2 = 0xFFFFFFFFu;
    e->call(0xAE8C, 0, 0, 0, 0, 0, ed, (uint32_t)(uintptr_t)&l2, 0, 0, 0);
    if (unit && d2rd32(unit) == 0) {
        const uint32_t id = d2rd32(unit + 0xC);
        const uint32_t v = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
        e->call(0x8B710, v & ~0xFFu, 0, id, 0, 0, v, flag, (uint32_t)code, l2);
    }
    if (flag) return;
    if (local) {
        const uint32_t sk = e->call(0xA49C, 0, 0, 0, 0, 0, local, 0xB20FB980u, 0xDB3);
        const uint32_t rw = e->call(0x1420, sk);
        if (rw && (d2rd8(bm + 0x14) & d2rd8(rw + 4))) {
            const int32_t s = (int16_t)d2rd16(rw + 0x80);
            if (s >= 0 || s < (int32_t)d2rd32(dt + 0xC4)) {
                const uint32_t it = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)s);
                if (it) {
                    e->call(0xABB6, 0, 0, 0, 0, 0, unit, it);
                    e->call(0xABAA, 0, 0, 0, 0, 0, it);
                }
                e->call(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)s, 0);
            }
            e->call(0xDF780, unit);
            if (unit) {
                for (uint32_t n = d2rd32(unit + 0xDC); n;) {
                    const uint32_t nx = d2rd32(n + 0x24);
                    if (d2rd8(n) == 8 && d2rd32(n + 0x14) == 0xFFFFFFFFu) e->call(0x8E6F0, n);
                    n = nx;
                }
            }
        }
    }
    uint32_t r = 0;
    if (code >= 0 && code < (int32_t)d2rd32(dt + 0xBA0)) r = (uint32_t)code * 0x23C + d2rd32(dt + 0xB98);
    if (d2rd8(bm + 0x14) & d2rd8(r + 4)) {
        const uint32_t fv = e->call(0xDF780, unit);
        e->call(0xA1D10, 0, 0, 0, 0, 0, fv, (uint32_t)code);
    }
}
