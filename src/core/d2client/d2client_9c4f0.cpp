// 1.13c D2Client+0x9C4F0 -- left-button listener of the bottom-bar panel (stdcall msg, ret 4). Input held off, no player or a dead player (mode 0x11):
// msg+0x18 = 0 and out. Otherwise 0x908C0, then the hit test 0x26320 (EAX = msg; nonzero: out, message untouched) and 0x8C580 (EAX = x = msg+0xC word,
// ECX = y = msg+0xE word; zero: out). The message is then handled (+0x18 = 1, +0x1C = 0); inside the box [0x10B3E4..3E8] x [0x10B3EC..3F0] the press
// goes to 0x98B60 (AL = 0 with EAX 0, ECX = y, EDX = x, stack = player, player+0x60) and a nonzero answer skips the cursor-mode tail; outside the box
// +0x18 is cleared again. Tail: with the panel mode [0x11BC34] nonzero a cursor mode [0x11B864] of 6 runs 0xBEB90 and 8 runs 0x14940 (AL 0x4C over the
// panel mode's upper bytes, ECX -1), each ending in the shared message tail 0xBE84; other cursor modes first run 0x8D300 when the panel mode is 2..4
// and then 0xBE84. With the panel mode 0 only the cursor-mode calls run, without 0xBE84, and any other value returns.
#include "cl_pa4.h"
extern "C" uint32_t d2c_9c4f0(uint32_t msg, const cl::Env& e) {
    using namespace pa4;
    const uint32_t pl = G(e, 0x11bbfc);
    if (G(e, 0x11c3a4) != 0 || !pl || e.R(pl + 0x10) == 0x11) { e.W(msg + 0x18, 0); return 0; }
    const uint32_t y = P16(e, msg + 0xe), x = P16(e, msg + 0xc);
    e.H(0x908c0);
    const uint32_t save = e.R(pl + 0x60);
    if (e.H(0x26320, 0, 0, 0, 0, 0, msg) != 0) return 0;
    if (e.H(0x8c580, 0, 0, 0, 0, 0, x, y) == 0) return 0;
    e.W(msg + 0x18, 1);
    e.W(msg + 0x1c, 0);
    uint32_t r = 0;
    if ((int32_t)x < (int32_t)G(e, 0x10b3e4) || (int32_t)x > (int32_t)G(e, 0x10b3e8) || (int32_t)y < (int32_t)G(e, 0x10b3ec) ||
        (int32_t)y > (int32_t)G(e, 0x10b3f0))
        e.W(msg + 0x18, 0);
    else
        r = e.H(0x98b60, pl, save, 0, 0, 0, 0, y, x);
    const uint32_t mode = G(e, 0x11bc34);
    const uint32_t cur = G(e, 0x11b864);
    if (r == 0) {
        if (mode == 0) {
            if (cur == 6) e.H(0xbeb90);
            else if (cur == 8) e.H(0x14940, 0, 0, 0, 0, 0, (cur & ~0xffu) | 0x4c, 0xffffffffu);
            return 0;
        }
        if (cur == 6) { e.H(0xbeb90); return e.H(0xbe84, msg); }
        if (cur == 8) { e.H(0x14940, 0, 0, 0, 0, 0, (mode & ~0xffu) | 0x4c, 0xffffffffu); return e.H(0xbe84, msg); }
    }
    if ((int32_t)mode >= 2 && (int32_t)mode <= 4) e.H(0x8d300);
    return e.H(0xbe84, msg);
}
