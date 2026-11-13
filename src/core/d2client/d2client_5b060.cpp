// 1.13c D2Client+0x5B060 -- mouse handler that arms the pending hireling slot (stack = message, ret 4; Ordinal 511 pops the message).
// g = pinned globals, one dword each: +0 [11C284] mode, +4 [11BBFC] player, +8 [11C4D4] list head (nodes: +4 type, +8, +0xC owner id, +0x20,
// +0x30 next), +0xC [10AA04] slot list head (entries: +0 kind, +0xC key, +0xE4 next).
// Mode 2: message +0x18 = 1, +0x1C = 0, Ordinal 511 (and the handler goes on). With a player and a list: the first node of type 7 owned by
// player+0xC with +0x20 == 0 and +8 != -1 sets message +0x18 = 0 and returns. Otherwise the first slot entry with key -1: kind 1 calls
// 0x511F0(player, 1) (ECX = player), message +0x18 = 1, +0x1C = 0, Ordinal 511; any other kind is retail's fatal path (id 0x5A, exit), never
// reached by the vector. No such entry: message +0x18 = 0.
#include "cl_env.h"
extern "C" void d2c_5b060(uint32_t msg, const cl::Env& e, uint32_t g) {
    if (e.R(g) == 2) { e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); e.H(0xbe84, msg); }
    uint32_t pl = e.R(g + 4);
    uint32_t n = e.R(g + 8);
    if (pl != 0 && n != 0) {
        uint32_t id = e.R(pl + 0xc);
        for (; n != 0; n = e.R(n + 0x30)) {
            if (e.R(n + 4) == 7 && e.R(n + 0xc) == id && e.R(n + 0x20) == 0) {
                if (e.R(n + 8) != 0xffffffffu) { e.W(msg + 0x18, 0); return; }
                break;
            }
        }
    }
    for (uint32_t s = e.R(g + 0xc); s != 0; s = e.R(s + 0xe4)) {
        if (e.R(s + 0xc) == 0xffffffffu) {
            if (e.R(s) != 1) { uint32_t m = e.H(0xbf92, 0x5a); e.H(0xbf80, 0xB2000000u | 0xD359F, m); e.H(0x3664, 0xffffffffu); return; }
            e.H(0x511f0, 1, 0, 0, 0, 0, 0, pl);
            e.W(msg + 0x18, 1); e.W(msg + 0x1c, 0); e.H(0xbe84, msg);
            return;
        }
    }
    e.W(msg + 0x18, 0);
}
