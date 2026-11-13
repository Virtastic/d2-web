// 1.13c D2Client+0x9C830 -- left-button-press listener of the in-game screen (stdcall msg, ret 4). Nothing when [0x11C3A4] is set, the player
// record [0x11BBFC] is none or its +0x10 is 0x11. The press (x = msg+0xC word, y = msg+0xE word) inside the box [0x10B3E4..0x10B3E8] x [0x10B3EC..0x10B3F0]
// goes to 0x9C4F0 and returns. Else 0x26320 (EAX = msg) equal to 1 returns unfinished. [0x11B864] 6 runs 0xBEB90, 8 runs 0x14940 (EAX = 0x4C, ECX = -1),
// both then finish (+0x18 = 1, +0x1C = 0, 0xBE84). Else 0xC202 (player+0x60) zero dispatches on the panel mode [0x11BC34]: 1 and 2 test the point against
// [0x10B2F8], [0x10B304] (inside: the message is finished, then in the box [0x10B3B4..B8] x [0x10B3BC..C0] with [0x11BC38] set the unit of kind 1 found by
// the id [0x11971D] in the 128-chain table 0x10A808 (link +0xE4, id +0xC) gets 0x95F80, else 0x8CF60 decides whether 0x8D300 runs); 0xE runs 0x8C440
// (EAX = x, ECX = y) then 0x23200; 0xC runs 0x8C4C0, 0x8C580 (-> 0x9C4F0) or 0x98B60 (EAX = 4, ECX = y, EDX = x). Other modes and a nonzero 0xC202 run
// 0x8D300 for the modes 2..4. Every path but the early ones finishes the message through the shared tail 0xBE84.
#include "cl_pa4.h"
extern "C" uint32_t d2c_9c830(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    if (G(e, 0x11c3a4) != 0) return 0;
    const uint32_t pl = G(e, 0x11bbfc);
    if (!pl || e.R(pl + 0x10) == 0x11) return 0;
    const uint32_t x = P16(e, msg + 0xc), y = P16(e, msg + 0xe);
    if ((int32_t)x >= (int32_t)G(e, 0x10b3e4) && (int32_t)x <= (int32_t)G(e, 0x10b3e8) && (int32_t)y >= (int32_t)G(e, 0x10b3ec) &&
        (int32_t)y <= (int32_t)G(e, 0x10b3f0)) {
        e.H(0x9c4f0, msg);
        return 0;
    }
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) == 1) return 0;
    const uint32_t st = G(e, 0x11b864);
    const auto t300 = [&]() { e.H(0x8d300); };
    if (st == 6) {
        e.H(0xbeb90);
        return fin(e, msg);
    }
    if (st == 8) {
        e.H(0x14940, 0, 0, 0, 0, 0, 0x4c, 0xFFFFFFFFu);
        return fin(e, msg);
    }
    const uint32_t busy = e.H(0xc202, e.R(pl + 0x60));
    const uint32_t mode = G(e, 0x11bc34);
    if (busy != 0) {
        if (mode >= 2 && mode <= 4) t300();
        return fin(e, msg);
    }
    if (mode == 1 || mode == 2) {
        if ((int32_t)x < 0 || (int32_t)x > (int32_t)G(e, 0x10b2f8) || (int32_t)y < 0 || (int32_t)y > (int32_t)G(e, 0x10b304)) return fin(e, msg);
        e.W(msg + 0x18, 1);
        e.W(msg + 0x1c, 0);
        e.H(0xbe84, msg);
        bool box2 = (int32_t)x >= (int32_t)G(e, 0x10b3b4) && (int32_t)x <= (int32_t)G(e, 0x10b3b8) && (int32_t)y >= (int32_t)G(e, 0x10b3bc) &&
                    (int32_t)y <= (int32_t)G(e, 0x10b3c0) && G(e, 0x11bc38) != 0;
        if (!box2) {
            if (e.H(0x8cf60) == 0) return fin(e, msg);
            t300();
            return fin(e, msg);
        }
        if (G(e, 0x119721) == 0) { t300(); return fin(e, msg); }
        const uint32_t id = G(e, 0x11971d);
        uint32_t u = G(e, 0x10a808 + (id & 0x7f) * 4);
        while (u && e.R(u + 0xc) != id) u = e.R(u + 0xe4);
        if (!u) { t300(); return fin(e, msg); }
        if (e.R(u) != 1) { assertAt(e, 0x5a); return 0; }
        e.H(0x95f80, u, 0, x, 0, 0, e.R(msg + 8), 0, y, 0, 1);
        return fin(e, msg);
    }
    if (mode == 0xe) {
        if (e.H(0x8c440, 0, 0, 0, 0, 0, x, y) == 0) return fin(e, msg);
        e.H(0x23200, pl, 0x13);
        return fin(e, msg);
    }
    if (mode == 0xc) {
        if (e.H(0x8c4c0, 0, 0, 0, 0, 0, x, y) == 0) return fin(e, msg);
        if (e.H(0x8c580, 0, 0, 0, 0, 0, x) != 0) {
            e.H(0x9c4f0, msg);
            return fin(e, msg);
        }
        if (e.H(0x8c4c0, 0, 0, 0, 0, 0, x, y) == 0) {
            e.H(0x23200, pl, 0x13);
            return fin(e, msg);
        }
        e.H(0x98b60, pl, e.R(pl + 0x60), 0, 0, 0, 4, y, x);
        return fin(e, msg);
    }
    if (mode >= 2 && mode <= 4) t300();
    return fin(e, msg);
}
