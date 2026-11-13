// 1.13c D2Client+0x9BA70 -- left-button-release listener of the panel mode 0xB (stdcall msg, ret 4). With the panel mode [0x11BC34] not 0xB only
// [0x11BEEC] clears. Otherwise the point (x = msg+0xC word, y = msg+0xE word) is stored ([0x10B2A4], [0x10B430]), the latches [0x11BEDC] and
// [0x11BEE0] clear and, when 0x26320 (EAX = msg) is zero, the player+0x60 value is tested through 0xC202 and then 0x8BD90 (ECX = x, EDX = y);
// a nonzero 0x8BD90 runs 0x59650 and returns, any other answer goes on to the box test. Inside the box x in ([0x10B28C], [0x10B290]),
// y in [[0x10B294], [0x10B298]] the message is finished (+0x18 = 1, +0x1C = 0, 0xBE84), 0x595C0 runs when [0x11BE64] is set and the state words
// [0x11BEEC], [0x11BE68], [0x11BE64] clear. Outside: 0x8B7F0 (EAX = x, EDX = y) nonzero runs 0x59600 when [0x11BE64] and [0x11BE6C] are set and
// clears [0x11BEEC], [0x11BE6C]; else 0x8B7A0 (same registers) nonzero runs 0x59650 and clears [0x11BE68] when [0x11BE68] and [0x11BE70] are set,
// then clears [0x11BE70], [0x11BEEC], [0x11BE6C]; else 0x26320 equal to 1 clears [0x11BE6C] and the three state words and returns without
// finishing the message; else 0x8D3D0 (EAX = y) nonzero with [0x11BE78] clear runs 0xC30E0(2), and [0x11BEEC] clear falls into the in-box finish.
// Otherwise [0x11BEEC] clears, [0x11BE68] (with [0x11BE70] set and [0x11BE6C] clear) or [0x11BE64] (= 1 -> cleared, 0x595C0) update, [0x11BE70]
// and [0x11BE6C] clear and the message is finished.
#include "cl_pa4.h"
extern "C" uint32_t d2c_9ba70(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (G(e, 0x11bc34) != 0xb) { S(e, 0x11beec, 0); return 0; }
    const uint32_t sv = e.R(G(e, 0x11bbfc) + 0x60);
    const uint32_t y = P16(e, msg + 0xe), x = P16(e, msg + 0xc);
    S(e, 0x10b2a4, x);
    S(e, 0x10b430, y);
    S(e, 0x11bedc, 0);
    S(e, 0x11bee0, 0);
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 0 && e.H(0xc202, sv) == 0 && e.H(0x8bd90, 0, 0, 0, 0, 0, 0, x, y) != 0) {
        e.H(0x59650);
        return 0;
    }
    const bool inBox = (int32_t)x > (int32_t)G(e, 0x10b28c) && (int32_t)x < (int32_t)G(e, 0x10b290) && (int32_t)y >= (int32_t)G(e, 0x10b294) &&
                       (int32_t)y <= (int32_t)G(e, 0x10b298);
    if (!inBox) {
        if (e.H(0x8b7f0, 0, 0, 0, 0, 0, x, 0, y) != 0) {
            if (G(e, 0x11be64) != 0 && G(e, 0x11be6c) != 0) e.H(0x59600);
            S(e, 0x11beec, 0);
            S(e, 0x11be6c, 0);
            return fin(e, msg);
        }
        if (e.H(0x8b7a0, 0, 0, 0, 0, 0, x, 0, y) != 0) {
            if (G(e, 0x11be68) != 0 && G(e, 0x11be70) != 0) {
                e.H(0x59650);
                S(e, 0x11be68, 0);
            }
            S(e, 0x11be70, 0);
            S(e, 0x11beec, 0);
            S(e, 0x11be6c, 0);
            return fin(e, msg);
        }
        if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) {
            S(e, 0x11be6c, 0);
            S(e, 0x11beec, 0);
            S(e, 0x11be68, 0);
            S(e, 0x11be64, 0);
            return 0;
        }
        const uint32_t hit = e.H(0x8d3d0, 0, 0, 0, 0, 0, y);
        e.Ej(0x8d3d0, x);
        if (hit != 0 && G(e, 0x11be78) == 0) e.H(0xc30e0, 2);
        if (G(e, 0x11beec) != 0) {
            S(e, 0x11beec, 0);
            if (G(e, 0x11be6c) == 0) {
                if (G(e, 0x11be70) != 0 && G(e, 0x11be68) != 0) S(e, 0x11be68, 0);
            } else if (G(e, 0x11be64) == 1) {
                S(e, 0x11be64, 0);
                e.H(0x595c0);
            }
            S(e, 0x11be70, 0);
            S(e, 0x11be6c, 0);
            return fin(e, msg);
        }
    }
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    e.H(0xbe84, msg);
    if (G(e, 0x11be64) != 0) e.H(0x595c0);
    S(e, 0x11beec, 0);
    S(e, 0x11be68, 0);
    S(e, 0x11be64, 0);
    return 0;
}
