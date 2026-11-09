// 1.13c D2Game+0x73BA0 -- PLRTRADE close of a trade pair. ESI = player 1, EDI = player 2, stack (game, t1, t2), ret 0xC;
// returns the last 0x8EA80 EAX. No 1.13c port existed (1.10f sub_6FC92920); written from the 1.13c disassembly.
// 0xD4570(EAX p) for both, 0xA9A6(p, 1) for both, +0xC8 |= 0x800000 on each non-null player, t1+0x5C -> +0xC = 0 and the
// same for t2, 0x716D0(game, p) for both. Packets through 0x8A3E0 (EAX = the player's client: 0xA4C0(p)+0x9C for a player
// unit, else 0; stack packet, length): {0x79, 0, 0 x4} and {0x79, 1, 0 x4} to p1, then the same pair to p2, then {0x77, 6}
// to p1 and p2 (retail builds this one in the game argument's slot). t1+0x50 = t2+0x50 = 5, 0xD4570 for both again, and
// event 0xD at frame+1 for p1 then p2 (0x8EA80(ECX game, EDX p; 0xD, frame+1, 0, 0, 0)).
// `wl` is the descriptor's `hookwlog` blob (the packet bytes; REGS B): count dword, then per send its length and bytes.
#include <cstring>
#include "../fn/hookenv.h"
namespace d2g {
static uint32_t client73ba0(HookEnv& e, uint32_t p)
{
    if (p && e.rd32(p) == 0) return e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, p) + 0x9C);
    return 0;
}
static void send73ba0(HookEnv& e, uint32_t wl, uint32_t cl, const uint8_t* b, uint32_t n)
{
    uint8_t buf[8] = {};
    memcpy(buf, b, n);
    e.h(0x8A3E0, cl, 0, 0, 0, 0, e.stk(buf), n);
    uint32_t cnt = e.rd32(wl), off = 4;
    for (uint32_t i = 0; i < cnt; i++) off += 4 + ((e.rd32(wl + off) + 3) & ~3u);
    e.wr32(wl + off, n);
    for (uint32_t i = 0; i < ((n + 3) & ~3u); i++) e.wr8(wl + off + 4 + i, i < n ? b[i] : 0);
    e.wr32(wl, cnt + 1);
}
uint32_t f73ba0(HookEnv& e, uint32_t p1, uint32_t p2, uint32_t game, uint32_t t1, uint32_t t2, uint32_t wl)
{
    e.h(0xD4570, p1);
    e.h(0xD4570, p2);
    e.h(0xA9A6, 0, 0, 0, 0, 0, p1, 1);
    e.h(0xA9A6, 0, 0, 0, 0, 0, p2, 1);
    if (p1) e.wr32(p1 + 0xC8, e.rd32(p1 + 0xC8) | 0x800000);
    if (p2) e.wr32(p2 + 0xC8, e.rd32(p2 + 0xC8) | 0x800000);
    e.wr32(e.rd32(t1 + 0x5C) + 0xC, 0);
    e.wr32(e.rd32(t2 + 0x5C) + 0xC, 0);
    e.h(0x716D0, 0, 0, 0, 0, 0, game, p1);
    e.h(0x716D0, 0, 0, 0, 0, 0, game, p2);
    const uint8_t k0[6] = { 0x79, 0, 0, 0, 0, 0 }, k1[6] = { 0x79, 1, 0, 0, 0, 0 }, k2[2] = { 0x77, 6 };
    uint32_t c = client73ba0(e, p1); send73ba0(e, wl, c, k0, 6);
    c = client73ba0(e, p1); send73ba0(e, wl, c, k1, 6);
    c = client73ba0(e, p2); send73ba0(e, wl, c, k0, 6);
    c = client73ba0(e, p2); send73ba0(e, wl, c, k1, 6);
    c = client73ba0(e, p1); send73ba0(e, wl, c, k2, 2);
    c = client73ba0(e, p2); send73ba0(e, wl, c, k2, 2);
    e.wr32(t1 + 0x50, 5);
    e.wr32(t2 + 0x50, 5);
    e.h(0xD4570, p1);
    e.h(0xD4570, p2);
    e.h(0x8EA80, 0, game, p1, 0, 0, 0xD, e.rd32(game + 0xA8) + 1, 0, 0, 0);
    return e.h(0x8EA80, 0, game, p2, 0, 0, 0xD, e.rd32(game + 0xA8) + 1, 0, 0, 0);
}
}
