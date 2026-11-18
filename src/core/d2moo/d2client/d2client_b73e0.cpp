// 1.13c D2Client+0xb73e0 -- client missile: timed bursts. ECX = unit, no return. No unit / class / row: 0xB1720().
// t = D2Common 0xC4D2(u). (1) Class word row +0x1E >= 0 and t == row +0x58: request 0x2001 at the unit's own spot
// (0x1630 / 0x1660, ECX = u), class row +0x1E, room from 0xA60D0 (unchecked, word 2 left 0); then row +0x60 more
// (re-read each pass) at spot - row +0x64 + 0x1330(EAX = 2 * row +0x64 + 1, ECX = seed +0x20) per axis.
// (2) Class word row +0x20 >= 0, t == row +0x5C and a room: room +0xC8 |= 0x40000; request 0x2000, room, unit, class
// row +0x20; a spawned missile with row +0x34 (word) > 0 gets 0x88A70(m, 0, 0, 0; EBX = that word). Tail 0xB17C0(ECX = u).
#include "cl_env.h"
extern "C" void d2c_b73e0(uint32_t u, const cl::Env& e) {
    uint32_t row = e.UnitRow(u);
    if (!row) { e.H(0xb1720); return; }
    uint32_t t = e.H(0xc4d2, u);
    uint32_t q[23];
    if (e.R16s(row + 0x1e) >= 0 && t == e.R(row + 0x58)) {
        uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
        uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
        for (auto& w : q) w = 0;
        q[0] = 0x2001;
        q[4] = (uint32_t)e.R16s(row + 0x1e);
        q[1] = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        q[11] = e.H(0xc388, u);
        q[12] = e.H(0xc3fa, u);
        q[5] = x; q[6] = y;
        e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
        uint32_t r = e.R(row + 0x64);
        if ((int32_t)e.R(row + 0x60) > 0) {
            uint32_t bx = x - r, by = y - r, md = r + r + 1;
            for (int32_t i = 0; i < (int32_t)e.R(row + 0x60); ++i) {
                q[5] = e.H(0x1330, 0, 0, 0, 0, 0, md, u + 0x20) + bx;
                q[6] = e.H(0x1330, 0, 0, 0, 0, 0, md, u + 0x20) + by;
                e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            }
        }
    }
    if (e.R16s(row + 0x20) >= 0 && t == e.R(row + 0x5c)) {
        uint32_t room = e.H(0xa60d0, 0, 0, 0, 0, 0, u);
        if (room) {
            e.W(room + 0xc8, e.R(room + 0xc8) | 0x40000);
            for (auto& w : q) w = 0;
            q[0] = 0x2000;
            q[4] = (uint32_t)e.R16s(row + 0x20);
            q[1] = room; q[2] = u;
            q[11] = e.H(0xc388, u);
            q[12] = e.H(0xc3fa, u);
            uint32_t m = e.HB(0xb30f0, (uint32_t)(uintptr_t)q);
            int32_t s = e.R16s(row + 0x34);
            if (m && s > 0) e.H(0x88a70, m, 0, 0, 0, 0, 0, 0, 0, (uint32_t)s);
        }
    }
    e.H(0xb17c0, 0, 0, 0, 0, 0, 0, u);
}
