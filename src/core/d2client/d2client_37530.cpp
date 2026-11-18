// 1.13c D2Client+0x37530 -- message handler (one stack word, the message, ret 4) for a click in the screen-corner button rectangle.
// Nothing (EAX as left by the last test) when the flag 0x11C454 is set or the cursor (x = 0x11B828, y = 0x11B824) is outside
// [w - 0x4B, w - 0x28] x [h - 0xC4, h - 0xA0] for the screen size words 0xDBC48 / 0xDBC4C. Otherwise: the cached slot 0xFAE08 is
// cleared and, if it held something, 0x3CC60 frees through the handle of string 0xD0960 (EAX = 2); then the slot pass of 0x3BA70 runs
// with toggle word 0xFAE04, object state +0x84 and no handler difference besides that 2, 3 and above 4 leave the loop for the tail
// (the pending word 0x113A68 jumps there too), the toggle word becomes (word == 0) unless the player starts with a nonzero dword
// or 0xA5440 answers nonzero, old 0 and new 1 calls 0x3CCC0 and old 1 and new 0 runs 0x358E0 then 0x3CC60 (EAX = 5, handle of
// string 0xD0924). The tail sets 0xF5204 := 1, calls 0xBEF6(0xD3588, 0xD6864, 1, 1), clears 0x11C450, and finishes the message
// (handled 1, result 0, tail 0xBE84). A slot count other than 0x26 takes the assert path (exits; the first one falls through to
// the second slot read with the old toggle word taken from the stack residue, not captured).
#include "cl_pa4.h"
extern "C" uint32_t d2c_37530(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    uint32_t eax = G(e, 0x11c454);
    if (eax) return eax;
    const int32_t w = (int32_t)G(e, 0xdbc48), h = (int32_t)G(e, 0xdbc4c), x = (int32_t)G(e, 0x11b828), y = (int32_t)G(e, 0x11b824);
    eax = (uint32_t)(w - 0x4b);
    if (x < (int32_t)eax) return eax;
    eax = (uint32_t)(w - 0x28);
    if (x > (int32_t)eax) return eax;
    eax = (uint32_t)y;
    if (y < h - 0xa0 - 0x24) return eax;
    if (y > h - 0xa0) return eax;
    uint32_t esi = G(e, 0xfae08);
    e.H(0xbf8c);
    uint32_t ebp = 0;
    bool assert1 = G(e, 0xdd5d0) != 0x26;
    if (!assert1) {
        S(e, 0xfae08, 0);
        if (esi) {
            const uint32_t hd = e.H(0xd0b4, 0xB2000000u | 0xD0960);
            e.H(0x3cc60, hd, 0, 0, 0, 0, 2);
        }
        ebp = G(e, 0xfae04);
        e.H(0xbf8c);
    }
    if (assert1 || G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfae04);
    bool tail = false;
    if (edx == 0) {
        if (G(e, 0x113a68)) tail = true;
        else {
            uint32_t i = 0;
            do {
                if (G(e, 0xFAD80 + 4 * i) == 1) {
                    eax = e.R(G(e, 0xDD538 + 4 * i) + 0x84);
                    if (eax > 4 || eax == 2 || eax == 3) { tail = true; break; }
                    if (eax == 1) {
                        e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                    } else if (eax == 4) {
                        if (G(e, 0x119721)) {
                            e.H(0x47b20);
                            e.Ej(0x47b20, G(e, 0x11971d));
                        }
                    }
                }
                ++i;
            } while ((int32_t)i < (int32_t)G(e, 0xdd5d0));
            if (!tail) edx = G(e, 0xfae04);
        }
    }
    if (!tail) {
        eax = G(e, 0x11bbfc);
        bool skip = false;
        if (eax) {
            if (e.R(eax) != 0) skip = true;
            else if (e.H(0xa5440)) skip = true;
        }
        if (!skip) { edx = (edx == 0); S(e, 0xfae04, edx); }
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
    S(e, 0xf5204, 1);
    e.H(0xbef6, 0x6FB83588u, 0x6FB86864u, 1, 1);
    e.W(msg + 0x18, 1);
    S(e, 0x11c450, 0);
    e.W(msg + 0x1c, 0);
    return e.H(0xbe84, msg);
}
