// 1.13c D2Client+0x9BC50 -- left-button-press listener of the panel mode 0xB (stdcall msg, ret 4). Other panel modes ([0x11BC34] != 0xB): nothing.
// The point (x = msg+0xC word, y = msg+0xE word) is stored ([0x10B2A4], [0x10B430]). 0x26320 (EAX = msg) zero with 0x8BD90 (ECX = x, EDX = y)
// nonzero sets the latch [0x11BEDC] := 1 and goes to the common finish (+0x18 = 1, +0x1C = 0, 0x88A70(0,0,0,0; EBX = 4), 0xBE84). Else 0x26320 equal to 1
// returns; inside the box x in [[0x10B2F8], [0x10B2FC]), y in [[0x10B300], [0x10B304]] or on a hit of 0x8C520 (EAX = x, ECX = y) the message goes to
// 0x98DC0 (its answer is returned). Otherwise 0xC202 (player+0x60) nonzero clears [0x11BE6C], [0x11BE64], [0x11BEEC], [0x11BE68], runs 0x595C0 and
// finishes the message. 0x8B7F0 (EAX = x, EDX = y) nonzero sets [0x11BE6C] and: with [0x11BE64] equal to 1 clears it, runs 0x595C0 and finishes the
// message; else sets [0x11BE64], [0x11BEEC], runs 0x88A70 and finishes it. 0x8B7A0 nonzero sets [0x11BE70], [0x11BE68], [0x11BEEC] and takes the common
// finish. 0x8D3D0 (EAX = y, EDI = x) nonzero sets [0x11BE7C], clears [0x11BE6C], [0x11BE64], [0x11BEEC], [0x11BE68], runs 0x88A70 and finishes the
// message, then clears [0x11BE70], [0x11BE6C]; otherwise 0x26320 equal to 1 clears [0x11BE64], [0x11BEEC], [0x11BE68], [0x11BE6C] and returns, any other
// answer finishes the message and clears [0x11BE70], [0x11BE6C].
#include "cl_pa4.h"
extern "C" uint32_t d2c_9bc50(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (G(e, 0x11bc34) != 0xb) return 0;
    const uint32_t x = P16(e, msg + 0xc), y = P16(e, msg + 0xe);
    S(e, 0x10b2a4, x);
    S(e, 0x10b430, y);
    const auto b4 = [&]() { e.H(0x88a70, 0, 0, 0, 0, 0, 0, 0, 0, 4); };
    const auto done = [&]() { e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); return e.H(0xbe84, msg); };
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 0 && e.H(0x8bd90, 0, 0, 0, 0, 0, 0, x, y) != 0) {
        S(e, 0x11bedc, 1);
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        b4();
        return e.H(0xbe84, msg);
    }
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) return 0;
    const bool inBox = (int32_t)x >= (int32_t)G(e, 0x10b2f8) && (int32_t)x < (int32_t)G(e, 0x10b2fc) && (int32_t)y >= (int32_t)G(e, 0x10b300) &&
                       (int32_t)y <= (int32_t)G(e, 0x10b304);
    if (inBox || e.H(0x8c520, 0, 0, 0, 0, 0, x, y) != 0) return e.H(0x98dc0, msg);
    if (e.H(0xc202, e.R(G(e, 0x11bbfc) + 0x60)) != 0) {
        S(e, 0x11be6c, 0);
        S(e, 0x11be64, 0);
        S(e, 0x11beec, 0);
        S(e, 0x11be68, 0);
        e.H(0x595c0);
        return done();
    }
    if (e.H(0x8b7f0, 0, 0, 0, 0, 0, x, 0, y) != 0) {
        const uint32_t cur = G(e, 0x11be64);
        S(e, 0x11be6c, 1);
        if (cur == 1) {
            S(e, 0x11be64, 0);
            e.H(0x595c0);
            return done();
        }
        S(e, 0x11be64, 1);
        S(e, 0x11beec, 1);
        b4();
        return done();
    }
    if (e.H(0x8b7a0, 0, 0, 0, 0, 0, x, 0, y) != 0) {
        S(e, 0x11be70, 1);
        S(e, 0x11be68, 1);
        S(e, 0x11beec, 1);
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        b4();
        return e.H(0xbe84, msg);
    }
    const uint32_t hit = e.H(0x8d3d0, 0, 0, 0, 0, 0, y);
    e.Ej(0x8d3d0, x);
    if (hit == 0) {
        if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) {
            S(e, 0x11be64, 0);
            S(e, 0x11beec, 0);
            S(e, 0x11be68, 0);
            S(e, 0x11be6c, 0);
            return 0;
        }
    } else {
        S(e, 0x11be7c, 1);
        S(e, 0x11be6c, 0);
        S(e, 0x11be64, 0);
        S(e, 0x11beec, 0);
        S(e, 0x11be68, 0);
        b4();
    }
    done();
    S(e, 0x11be70, 0);
    S(e, 0x11be6c, 0);
    return 0;
}
