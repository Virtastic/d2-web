// 1.13c D2Client+0xA2950: EAX = unit, six stack arguments (a0, skill index, a2, a3, a4, a5), ret 0x18, no result.
// Looks the skill row up (+0x80 room-list index, +0x54 view index), finds or creates the unit's room, then walks the row's
// handler list at +0xF6 (a5 - 1 steps) calling the handler table entries (D2Client+0xDEA48, count at +0xDEC50) and finally the
// row's effect handler, with the 0xA1340 position saved and restored around it. The second 0xA1340 reuses the room and a0 locals.
// ours: env.gl = the handler table (0xDEA48; 0xDEC50 is gl + 0x208).
#include "cl_a2.h"
extern "C" void d2cl_a2950(uint32_t unit, uint32_t a0, int32_t idx, uint32_t a2, uint32_t a3, uint32_t a4, int32_t a5,
                           const cl::Env& e) {
    if (!unit) return;
    if (idx < 0 || idx >= (int32_t)e.R(e.dt + 0xba0)) return;
    const uint32_t rec = e.R(e.dt + 0xb98) + (uint32_t)idx * 0x23c;
    if (!rec) return;
    const int32_t s80 = e.R16s(rec + 0x80);
    if (s80 < 0 || s80 >= (int32_t)e.R(e.dt + 0xc4)) return;
    const int32_t s54 = e.R16s(rec + 0x54);
    if (s54 < 0 || s54 >= (int32_t)e.R(e.dt + 0xbd4)) return;
    uint32_t room = e.H(0xc1c0, unit, (uint32_t)s80);
    if (!room) {
        room = e.H(0xc442, 0, 0, 0, e.R(unit), e.R(unit + 0xc));
        if (!room) return;
        e.H(0xc2f2, room, (uint32_t)s80);
        e.H(0xc36a, unit, room, 1);
        e.H(0xc562, room, (uint32_t)s54, (uint32_t)a5, 0);
    }
    uint32_t xo = 0, yo = 0;
    e.Ed((uint32_t)(uintptr_t)&xo);
    const uint32_t ok1 = e.H(0xa1340, (uint32_t)(uintptr_t)&yo, 0, 0, 0, 0, 0, 0, 0, unit);
    const uint32_t holder = e.H(0xa5f70);
    if (a0) e.H(0xc62e, unit, a0);
    else if (a3 && a4) e.H(0xc3ee, e.R(unit + 0x2c), a3, a4);
    const uint32_t sv28 = e.R(unit + 0x28);
    if (a3 && !a4) e.W(unit + 0x28, a3);
    const int32_t n = a5 - 1;
    const uint32_t tbl = e.gl;
    if ((e.R8(rec + 4) & e.R8(e.bm + 0x1c))) {
        for (int32_t i = 0; i < n; ++i) {
            const uint32_t hnd = e.R(tbl + 4u * (uint32_t)e.R16s(rec + 0xf6 + 2u * (uint32_t)i));
            if (hnd) {
                e.H(0xc562, room, (uint32_t)s54, (uint32_t)i + 1, 0);
                e.H(hnd, a2, 0, 0, 0, 0, 0, unit, (uint32_t)idx);
            }
        }
        e.H(0xc562, room, (uint32_t)s54, (uint32_t)n + 1, 0);
    }
    const int32_t hv = e.R16s(rec + 0xf6 + 2u * (uint32_t)n);
    if (hv >= 0 && hv < (int32_t)e.R(tbl + 0x208)) {
        const int32_t e8 = e.R16s(rec + 0xe8);
        if (e8 > 0) {
            if (e.R8(e.bm + 4) & e.R8(rec + 4)) {
                e.Ed((uint32_t)(uintptr_t)&room);
                e.H(0xa1340, (uint32_t)(uintptr_t)&a0, 0, 0, 0, 0, 0, 0, 0, unit);
                const uint32_t w[8] = { (uint32_t)e8, 0, 0, room, a0, (uint32_t)idx, a2, 0 };
                cl::b9a80(e, unit, w);
            } else {
                e.H(0xa15c0, (uint32_t)e8, (uint32_t)idx, a2, 0, 0, unit);
            }
            e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
        }
        const uint32_t hnd = e.R(tbl + 4u * (uint32_t)e.R16s(rec + 0xf6 + 2u * (uint32_t)n));
        if (!hnd) e.W(unit + 0xc4, e.R(unit + 0xc4) | 0x40);
        else if (e.H(hnd, a2, 0, 0, 0, 0, 0, unit, (uint32_t)idx)) cl::a2Effects(e, unit, rec);
    }
    e.H(0xc62e, unit, holder);
    if (ok1) e.H(0xc3ee, e.R(unit + 0x2c), xo, yo);
    if (a3 && !a4) e.W(unit + 0x28, sv28);
}
