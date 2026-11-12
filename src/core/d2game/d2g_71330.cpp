// 1.13c D2Game+0x71330 -- PLRTRADE_RequestTrade: ECX = game, EDX = requesting player, stack = target record (or 0), ret 4;
// no result. `now` is GetTickCount (the FN descriptor pins it), `wl` the `hookwlog` blob the 0x8A3E0 packet bytes go to.
// HookEnv form of d2game_71330.cpp, resolved against the 1.13c disassembly.
// A player already trading (byte +0x6C and +0x68 set) or whose id (+0x64) is not in game+0x1120[id & 0x7F] (chain +0xE4,
// match +0xC) gets {0x77, 0x0C}. Otherwise key = target ? target+0xC : -1; with a = 0xA4C0(unit), a+0x5C and its list
// (+8, next +8, key +0, value +4): the match runs 0xD62E0(ECX other, EAX 0; game, value, &scratch, 1, 1, 1) and on nonzero
// 0x6FFA0(EAX unit). Then pa = 0xA4C0(unit), pb = 0xA4C0(other), both +0x5C -> +0x14 = now + 10000, {0x77, 0x0E} to the
// player then the other; with pa+0x50 or pb+0x50 == 4: {0x79, 0/1, 0 x4} to each and 0x70B70(EAX unit; pa, pb).
// The null-game assert (_exit) is not framed.
#include "../fn/hookenv.h"
namespace d2g {
static uint32_t client71330(HookEnv& e, uint32_t p)
{
    if (e.rd32(p) == 0) return e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, p) + 0x9C);
    return 0;
}
static void send71330(HookEnv& e, uint32_t wl, uint32_t p, const uint8_t* b, uint32_t n)
{
    const uint32_t cl = client71330(e, p);
    uint8_t buf[8] = {};
    for (uint32_t i = 0; i < n; i++) buf[i] = b[i];
    e.h(0x8A3E0, cl, 0, 0, 0, 0, e.stk(buf), n);
    uint32_t cnt = e.rd32(wl), off = 4;
    for (uint32_t i = 0; i < cnt; i++) off += 4 + ((e.rd32(wl + off) + 3) & ~3u);
    e.wr32(wl + off, n);
    for (uint32_t i = 0; i < ((n + 3) & ~3u); i++) e.wr8(wl + off + 4 + i, i < n ? b[i] : 0);
    e.wr32(wl, cnt + 1);
}
void f71330(HookEnv& e, uint32_t game, uint32_t unit, uint32_t target, uint32_t now, uint32_t wl)
{
    static const uint8_t kBusy[2] = { 0x77, 0x0C }, kOpen[2] = { 0x77, 0x0E };
    static const uint8_t k0[6] = { 0x79, 0, 0, 0, 0, 0 }, k1[6] = { 0x79, 1, 0, 0, 0, 0 };
    const uint32_t id = e.rd32(unit + 0x64);
    if (e.rd8(unit + 0x6C) && e.rd32(unit + 0x68)) { send71330(e, wl, unit, kBusy, 2); return; }
    uint32_t other = e.rd32(game + 0x1120 + (id & 0x7F) * 4);
    while (other && e.rd32(other + 0xC) != id) other = e.rd32(other + 0xE4);
    if (!other) { send71330(e, wl, unit, kBusy, 2); return; }
    const uint32_t key = target ? e.rd32(target + 0xC) : 0xFFFFFFFFu;
    const uint32_t a = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (a && e.rd32(a + 0x5C)) {
        for (uint32_t n = e.rd32(e.rd32(a + 0x5C) + 8); n; n = e.rd32(n + 8)) {
            if (e.rd32(n) != key) continue;
            uint32_t scratch[3] = {};
            if (e.h(0xD62E0, 0, other, 0, 0, 0, game, e.rd32(n + 4), e.stk(scratch), 1, 1, 1)) e.h(0x6FFA0, unit);
            break;
        }
    }
    const uint32_t pa = e.h(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t pb = e.h(0xA4C0, 0, 0, 0, 0, 0, other);
    const uint32_t xa = e.rd32(pa + 0x5C), xb = e.rd32(pb + 0x5C);
    e.wr32(xa + 0x14, now + 10000);
    e.wr32(xb + 0x14, now + 10000);
    send71330(e, wl, unit, kOpen, 2);
    send71330(e, wl, other, kOpen, 2);
    if (e.rd32(pa + 0x50) != 4 && e.rd32(pb + 0x50) != 4) return;
    send71330(e, wl, unit, k0, 6);
    send71330(e, wl, unit, k1, 6);
    send71330(e, wl, other, k0, 6);
    send71330(e, wl, other, k1, 6);
    e.h(0x70B70, unit, 0, 0, 0, 0, pa, pb);
}
}
