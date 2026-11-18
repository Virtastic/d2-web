// 1.13c D2Client+0xb9740 -- client missile: disc scan spawning on clear cells. Stack unit, class table, pick, radius,
// extra, chance (ret 0x18), no return. 0xC1F0(u) == 0 returns. Request word 0 = 1 (0x8001 when extra != 0), 1 =
// 0xA60D0(EAX = u), 11/12 = 0xC388/0xC3FA(u); base x0/y0 = the unit path +0xC/+0x10 (static-path types) or
// 0xC3BE/0xC2EC(path). For i, j in [-r, r] (i outer) with x = x0+i, y = y0+j: room = 0xC1F0(u), skip on 0 or when
// i*i + j*j > r*r; 0xC436(room, &{x, y}, &{x+i, y+j}, 4) must be 0; 0x1400(EAX = 0xC1F0(u), EBX = x, EDI = y) must be
// non-zero and 0xC208 of it 0. Then with extra != 0 word 19 = (seed step on u +0x20) % 25 + extra - 12; words 5/6 =
// x/y, word 4 = table[0x1330(EAX = pick, ECX = u +0x20)]; one more seed step: % 100 < chance sends 0xB30F0(ECX = block).
#include "cl_env.h"
extern "C" void d2c_b9740(uint32_t u, uint32_t tbl, uint32_t pick, int32_t r, uint32_t extra, int32_t chance, const cl::Env& e) {
    if (!e.H(0xc1f0, u)) return;
    uint32_t sd = u + 0x20;
    if (!sd) return;
    uint32_t q[23] = {};
    q[0] = extra ? 0x8001 : 1;
    q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
    q[11] = e.H(0xc388, u);
    q[12] = e.H(0xc3fa, u);
    uint32_t x0 = e.PD(u, 0xc3be, 0xc);
    uint32_t y0 = e.PD(u, 0xc2ec, 0x10);
    int32_t rr = (int32_t)((uint32_t)r * (uint32_t)r);
    for (int32_t i = 0 - r; i <= r; ++i) {
        uint32_t x = x0 + (uint32_t)i;
        for (int32_t j = 0 - r; j <= r; ++j) {
            uint32_t y = y0 + (uint32_t)j;
            uint32_t room = e.H(0xc1f0, u);
            if (!room) continue;
            if ((int32_t)((uint32_t)j * (uint32_t)j + (uint32_t)i * (uint32_t)i) > rr) continue;
            uint32_t f[4] = { x + (uint32_t)i, y + (uint32_t)j, x, y };   // retail's frame: target pair, then the origin pair 8 bytes up
            uint32_t los = e.H(0xc436, room, (uint32_t)(uintptr_t)(f + 2), (uint32_t)(uintptr_t)f, 4);
            if (e.blk) e.blk(e.ctx, 0xc436, (uint32_t)(uintptr_t)f, 4);
            if (los) continue;
            uint32_t rm = e.H(0xc1f0, u);
            uint32_t c = e.H(0x1400, 0, 0, 0, 0, 0, rm, 0, 0, x);
            e.Edj(0x1400, y);
            if (!c || e.H(0xc208, c)) continue;
            if (extra) q[19] = e.Lcg(sd) % 25u + extra - 12;
            q[5] = x;
            q[6] = y;
            q[4] = e.R(tbl + e.H(0x1330, 0, 0, 0, 0, 0, pick, sd) * 4);
            if ((int32_t)(e.Lcg(sd) % 100u) < chance) e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        }
    }
}
