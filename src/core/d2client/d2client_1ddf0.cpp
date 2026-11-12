// 1.13c D2Client+0x1DDF0 -- mouse-down message handler of the belt/inventory strip (stdcall, msg; EAX is live on entry because the
// first exit leaves only AL replaced). Gates: byte 0x6FBBF2F3 set, x right of half the screen, y below the strip (0xD0AE == 2 selects the
// short strip). Then x/y are moved into panel space and either the held-button state is set from the slot under the point
// (0xCCA60 by y, 0xCC7A0, 0xCC770/0xCC800 by x,y) or a slot is picked (0xCCA00, 0xCCCD0, 0xCDA90, 0xCC830); every path ends in the tail.
#include "d2client_sn18.h"
extern "C" uint32_t d2c_1ddf0(uint32_t eax0, uint32_t msg, const cl::Env& e) {
    const int32_t x0 = (int32_t)sn18::R16(e, msg + 0xc), y0 = (int32_t)sn18::R16(e, msg + 0xe);
    if (e.R8(0x6fbbf2f3)) return (eax0 & ~0xffu) | e.R8(0x6fbbf2f3);
    const int32_t W = (int32_t)e.R(0x6fb8bc48), half = (W - (W >> 31)) >> 1;
    if (x0 > half) return (uint32_t)half;
    const uint32_t r = e.H(0xD0AE);
    const int32_t H = (int32_t)e.R(0x6fb8bc4c);
    if (r == 2) { if (y0 > H - 0x2f) return (uint32_t)(H - 0x2f); }
    else if (y0 > H - 0x31) return r;
    const int32_t x = x0 - (int32_t)e.R(0x6fbcb9a0), y = y0 + (int32_t)e.R(0x6fbcb9a4);
    if (e.R8(0x6fbd23f3)) return sn18::tail(e, msg);
    int32_t slot = (int32_t)e.H(0xCCA60, 0, 0, 0, 0, 0, (uint32_t)y);
    if (slot == -1) {
        slot = (int32_t)e.H(0xCC7A0);
        if (slot != -1 && slot < (int32_t)e.R(0x6fbd23e0)) {
            uint32_t p = 0x6fbd274d;
            for (int32_t i = 0; i < (int32_t)e.R(0x6fbd23e0); i++, p += 0x26a) {
                if ((uint32_t)slot == e.R8(p - 3) && e.R8(p)) {
                    e.W(0x6fbd23f1, (e.R(0x6fbd23f1) & ~0xffu) | 1);
                    e.W(0x6fbd23f5, (uint32_t)slot);
                    e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
                }
            }
            return sn18::tail(e, msg);
        }
        if (e.H(0xCC770, 0, 0, 0, 0, 0, (uint32_t)x, (uint32_t)y) & 0xff) {
            e.W(0x6fbd23f2, (e.R(0x6fbd23f2) & ~0xffu) | 1);
            e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
        } else if (e.H(0xCC800, 0, 0, 0, 0, 0, (uint32_t)x, (uint32_t)y) & 0xff) {
            e.W(0x6fbd23f4, (e.R(0x6fbd23f4) & ~0xffu) | 1);
            e.H(0x88A70, 0, 0, 0, 0, 0, 0, 0, 0, 4);
        }
        return sn18::tail(e, msg);
    }
    if (slot < 5 && (uint32_t)slot != e.R(0x6fbd3395)) {
        const bool a = e.H(0xBF8C) != 0;
        if (!((!a || !e.R(0x6fbc9854)) && slot == 4)) {
            e.H(0xCCA00);
            e.H(0xCCCD0, 0, 0, 0, 0, 0, (uint32_t)slot);
            e.H(0xCDA90, e.R(0x6fbd3395), 1);
            e.H(0xCC830);
        }
    }
    return sn18::tail(e, msg);
}
