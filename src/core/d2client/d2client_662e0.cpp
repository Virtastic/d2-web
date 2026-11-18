// 1.13c D2Client+0x662E0 -- stdcall(msg) mouse-message handler for the strip of eight/seven buttons above the panel: with the pending flag 0x11C09C set it
// clears it, and for a player whose inventory exists and for which the Storm check 0xC202 says zero it sets the panel state, clears the button
// scratch table at 0x115550, and tests the cursor (msg +0xC x, +0xE y) against the strip (centred on half the screen width, shifted by the three
// layout flags); inside it, every button whose x-span holds the cursor and is either type 7 or enabled for a live player is hit through 0x65E70, the
// click scratch is cleared, msg +0x18 := 1, +0x1C := 0 and the message goes to Storm ordinal 511 (a tail call); outside it msg +0x18 := 0.
// EAX on return: the player (flag clear) / 0 / the Storm answer / 7 (input held off) / msg / the ordinal 511 answer.
#include "cl_cmd.h"
extern "C" uint32_t d2c_662e0(uint32_t msg, const cl::Env& e) {
    using namespace clc;
    const uint32_t flag = G(e, 0x11c09c);
    const uint32_t pl0 = G(e, 0x11bbfc);
    if (!flag) return pl0;
    S(e, 0x11c09c, 0);
    if (!pl0) return 0;
    const uint32_t inv = e.R(pl0 + 0x60);
    if (!inv) return 0;
    const uint32_t rc = e.H(0xc202, inv);
    if (rc) return rc;
    if (G(e, 0x11b864) != 7) {
        S(e, 0x11b834, 0); S(e, 0x11b864, 1); S(e, 0x11b854, 5); S(e, 0x11b858, 0);
    }
    const int32_t n = G(e, 0x11c098) ? 7 : 8;
    if (G(e, 0x11c090)) { S(e, 0x11c094, 0); return 7; }
    const int32_t x = (int32_t)(e.R(msg + 0xc) & 0xffff), y = (int32_t)(e.R(msg + 0xe) & 0xffff);
    const uint32_t ebp = G(e, 0x11bbfc);
    for (int32_t i = 0; i < n; ++i) S(e, 0x115550 + 4 * (uint32_t)i, 0);
    int32_t esi = 0;
    if (G(e, 0x11c474) && (int32_t)e.H(0x26500) > 1) esi = -0x76;
    if (G(e, 0x11c088)) esi -= 0x76;
    if (G(e, 0x11c08c)) esi += 0x77;
    const int32_t base = (int32_t)G(e, 0xdbc48) / 2 + esi;
    const int32_t h = (int32_t)G(e, 0xdbc4c);
    if (x > base - 0x54 && x < base + 0x59 && y > h - 0x45 && y < h - 0x2f) {
        for (int32_t i = 0; i < n; ++i) {
            const int32_t a = (int32_t)G(e, 0x1154f0 + 4 * (uint32_t)i);
            if (x > a && x < a + 0x14 &&
                (G(e, 0x115530 + 4 * (uint32_t)i) == 7 || (ebp && e.R(ebp) == 0 && e.R(ebp + 0x10) != 0x11)))
                e.H(0x65e70);
        }
        S(e, 0x11c3dc, 0); S(e, 0x11c3e0, 0); S(e, 0x11c3f8, 0); S(e, 0x11c3fc, 0);
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        return e.H(0xbe84, msg);
    }
    e.W(msg + 0x18, 0);
    S(e, 0x11c094, 0);
    return msg;
}
