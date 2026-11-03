// 1.13c D2Game+0x526F0 (Server_ClientTimeoutCheck: per-message game check that kicks a stalled or idle game's clients):
// stdcall (id, msg), ret 8, no result. Written from the 1.13c disassembly as a rule over E (HookEnv).
// Module globals (pinned in the capture): T = 0x110318, 32 slot pointers picked by id & 0x1F; L = 0x110398 list head (+0 first node + 0x10,
// +4 tail; empty when +4 is the address of L itself), L+0x14 a pending-message count. The bucket locks 0x111C58 + 0x18 * k and the global lock
// 0x111F58 go through EnterCriticalSection / LeaveCriticalSection (no-ops in a single-threaded capture, not logged).
// game := 0x2AAE0(msg+8) (stdcall, locks the game); none returns. 0x2E050(EAX game); every client c of game+0x88 (next c+0x4A8, read first)
// gets 0x2D500(EAX c, game, 1). t := 0xA2F2 (Fog 10055). Kick when game+0x1DC4 == 1 or t - game+0x1DC4 > 0x708 (unsigned): every client then
// gets the one-byte packets 0xB0 (0xA35E: D2Net 10002(0, c+0, &b, 1)) and 6 (10002(1, c+0, &b, 1)) and 0x4B7F0(EAX c+0, game, 0).
// Otherwise now := GetTickCount: game+0x8C set clears the idle stamp game+0x1DC0 (no kick); else a zero stamp := now, and it kicks when
// now - stamp > 300000 or 0x5DC < (int)game+0xA8 < 0x1D4C.
// Both paths need the game lock game+0x18 (a null one is the 0x100F assert, not modelled) and leave it.
// No kick: slot s := T[id & 0x1F]; s set gets 0x52160(EAX 0x28, EDX msg, s) (s+0 >= 0x20 is the 0x96 assert, not modelled).
// Kick: 0x2CE20(EAX msg+8); with s set and the list head h = L+0 (node n = h - 0x10): n+4 == s+4 + 1 unlinks p = n+8 (0x1E50(EAX p)),
// 0x52160(EAX 0, EDX p - 0xC, s), n+4 -= 1 and 0x1E50(EAX h), x := h; else s+4 -= 1, 0x1E50(EAX s + 0x10), x := s + 0x10. Then
// 0x1E10(EAX x, ECX &L) and Fog 10043 free 0xA322(ECX msg, EDX file, 0x306, 0). L+0x14 -= 1 on every kick.
// (The empty-list 0x2E9 and bad-node 0x2F2 asserts are not modelled.)
#pragma once
#include <cstdint>

namespace g526f0 {
template <class E> void ClientTimeoutCheck(E& e, uint32_t tb, uint32_t lh, uint32_t now, uint32_t id, uint32_t msg)
{
    // 1.13c D2Game+0x526F0
    const uint32_t key = e.rd32(msg + 8);
    const uint32_t game = e.h(0x2AAE0, 0, 0, 0, 0, 0, key);
    if (!game) return;
    e.h(0x2E050, game);
    for (uint32_t c = e.rd32(game + 0x88); c;) { const uint32_t nx = e.rd32(c + 0x4A8); e.h(0x2D500, c, 0, 0, 0, 0, game, 1); c = nx; }
    const uint32_t t = e.h(0xA2F2), st = e.rd32(game + 0x1DC4);
    bool kick;
    if (st == 1 || t - st > 0x708) {
        for (uint32_t c = e.rd32(game + 0x88); c;) {
            const uint32_t nx = e.rd32(c + 0x4A8);
            uint32_t b = 0xB0;
            e.h(0xA35E, 0, 0, 0, 0, 0, 0, e.rd32(c), e.stk(&b), 1); e.blk(0xA35E, &b, 1);
            b = 6;
            e.h(0xA35E, 0, 0, 0, 0, 0, 1, e.rd32(c), e.stk(&b), 1); e.blk(0xA35E, &b, 1);
            e.h(0x4B7F0, e.rd32(c), 0, 0, 0, 0, game, 0);
            c = nx;
        }
        kick = true;
    } else if (e.rd32(game + 0x8C)) {
        e.wr32(game + 0x1DC0, 0); kick = false;
    } else {
        if (!e.rd32(game + 0x1DC0)) e.wr32(game + 0x1DC0, now);
        if (now - e.rd32(game + 0x1DC0) > 0x493E0) kick = true;
        else { const int32_t v = (int32_t)e.rd32(game + 0xA8); kick = v > 0x5DC && v < 0x1D4C; }
    }
    const uint32_t s = e.rd32(tb + 4 * (id & 0x1F));
    if (!kick) {
        if (s) e.h(0x52160, 0x28, 0, msg, 0, 0, s);
        return;
    }
    e.h(0x2CE20, key);
    if (s) {
        const uint32_t h = e.rd32(lh), n = h - 0x10;
        uint32_t x;
        if (e.rd32(n + 4) == e.rd32(s + 4) + 1) {
            const uint32_t p = e.rd32(n + 8);
            e.h(0x1E50, p);
            e.h(0x52160, 0, 0, p - 0xC, 0, 0, s);
            e.wr32(n + 4, e.rd32(n + 4) - 1);
            e.h(0x1E50, h); x = h;
        } else {
            e.wr32(s + 4, e.rd32(s + 4) - 1);
            e.h(0x1E50, s + 0x10); x = s + 0x10;
        }
        e.h(0x1E10, x, 0xB2110398u);                                  // ECX = &L, as the capture tags a module address
        e.h(0xA322, 0, msg, 0xB20FC15Cu, 0, 0, 0x306, 0);              // EDX = the source-file string
    }
    e.wr32(lh + 0x14, e.rd32(lh + 0x14) - 1);
}
}
