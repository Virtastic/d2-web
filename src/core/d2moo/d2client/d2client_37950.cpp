// 1.13c D2Client+0x37950 -- message handler (one stack word, the message, ret 4) for a click on one of two small buttons of the panel
// strip. The cursor is the two words msg+0xC (x) and msg+0xE (y). With the flag 0x11C448 clear, or the click outside the active
// rectangle, the click state words (0x11C448, 0x11C3DC, 0x11C3E0, 0x11C3F8, 0x11C3FC) are cleared and the message finished
// (+0x1C = 0, +0x18 = 1, 0xBE84). Otherwise the import 0xD0AE decides: 2 selects the rectangle x 0x2AD..0x2CD, y 0x18..0x38, anything else
// x 0x20D..0x22D, same y. Both run the slot pass of 0x3BA70 on the toggle word 0xFAE04 (object state +0x84; the pending word 0x113A68
// and states 2, 3, above 4 skip the loop) and the same toggle rule (old 0 / new 1: 0x3CCC0, old 1 / new 0: 0x358E0 then 0x3CC60,
// EAX = 5 and the handle of string 0xD0924 from 0xD0B4). The 2 branch always ends in 0x358E0 (once, after 0x3CCC0 too) and the
// finish with 0xBE84; the other branch ends in 0x358E0 and a tail jump to 0xBE84 with the stores in the other order.
// A slot count other than 0x26 takes the assert path (exits, not captured).
#include "cl_pa4.h"
namespace {
bool pass37950(const cl::Env& e, uint32_t& edx) {
    using namespace pa4;
    edx = G(e, 0xfae04);
    if (edx != 0) return false;
    if (G(e, 0x113a68)) return true;
    uint32_t i = 0;
    do {
        if (G(e, 0xFAD80 + 4 * i) == 1) {
            const uint32_t st = e.R(G(e, 0xDD538 + 4 * i) + 0x84);
            if (st > 4 || st == 2 || st == 3) return true;
            if (st == 1) e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
            else if (st == 4 && G(e, 0x119721)) { e.H(0x47b20); e.Ej(0x47b20, G(e, 0x11971d)); }
        }
        ++i;
    } while ((int32_t)i < (int32_t)G(e, 0xdd5d0));
    edx = G(e, 0xfae04);
    return false;
}
void toggle37950(const cl::Env& e, uint32_t& edx) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    bool skip = false;
    if (pl) {
        if (e.R(pl) != 0) skip = true;
        else { edx = e.H(0xa5440); if (edx) skip = true; }   // the recorder thunk leaves EDX = the hook's return value
    }
    if (!skip) { edx = (edx == 0); S(e, 0xfae04, edx); }
}
void assert37950(const cl::Env& e) {
    const uint32_t r = e.H(0xbf92, 0x139b);
    e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
    e.H(0x3664, 0xFFFFFFFFu);
}
}  // namespace
extern "C" uint32_t d2c_37950(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t x = e.R(msg + 0xc) & 0xffff, y = e.R(msg + 0xe) & 0xffff;
    bool reset = G(e, 0x11c448) == 0;
    bool second = false;
    if (!reset) {
        const uint32_t k = e.H(0xd0ae);
        if (k == 2) {
            second = true;
            if (x < 0x2ad || x > 0x2cd || y < 0x18 || y > 0x38) reset = true;
        } else if (x < 0x20d || x > 0x22d || y < 0x18 || y > 0x38) reset = true;
    }
    if (!reset && second) {
        const uint32_t ebp = G(e, 0xfae04);
        e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) assert37950(e);
        uint32_t edx;
        const bool tail = pass37950(e, edx);
        bool after = false;
        if (!tail) {
            toggle37950(e, edx);
            if (ebp == 0) {
                if (edx != 0) {
                    const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD0924);
                    e.H(0x3ccc0, hd, 0, 0, 0, 0, 5);
                    after = true;
                    e.H(0x358e0);
                }
            } else if (edx == 0) {
                e.H(0x358e0);
                const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD0924);
                e.H(0x3cc60, hd, 0, 0, 0, 0, 5);
            }
        }
        if (!after) e.H(0x358e0);
    } else if (!reset) {
        const uint32_t ebp = G(e, 0xfae04);
        e.H(0xbf8c);
        if (G(e, 0xdd5d0) != 0x26) assert37950(e);
        uint32_t edx;
        const bool tail = pass37950(e, edx);
        if (!tail) {
            toggle37950(e, edx);
            if (ebp == 0) {
                if (edx != 0) {
                    const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD0924);
                    e.H(0x3ccc0, hd, 0, 0, 0, 0, 5);
                }
            } else if (edx == 0) {
                e.H(0x358e0);
                const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD0924);
                e.H(0x3cc60, hd, 0, 0, 0, 0, 5);
            }
        }
        e.H(0x358e0);
        e.W(msg + 0x1c, 0);
        e.W(msg + 0x18, 1);
        return e.H(0xbe84, msg);
    }
    if (reset) {
        S(e, 0x11c448, 0);
        S(e, 0x11c3dc, 0);
        S(e, 0x11c3e0, 0);
        S(e, 0x11c3f8, 0);
        S(e, 0x11c3fc, 0);
    }
    e.W(msg + 0x1c, 0);
    e.W(msg + 0x18, 1);
    return e.H(0xbe84, msg);
}
