// 1.13c D2Game+0xB8460: ECX = game, EDX = unit, stack ctx (ret 4). The Baal-clone think: quits (0xDEEE0) for a missing
// unit, a dead owner, a unit without its clone flag (unit+0xC8 bit 10) or a dead/missing master (unit+0x94/+0x98);
// otherwise asks 0xB7090 for a command (two out-words), finds or creates the unit's type-10 AI node (the circular list
// at [[unit+0x14]+0x28]+0x20; created from a 5-dword request {?, ?, 10, x, y} through 0x21290 / 0x20E00), picks a mode
// with 0xB79B0 (7/9/14/15 become 2) and runs it through the dispatcher 0xB8070, or idles 15 frames without a command.
// The request's first two words are uninitialised in retail (masked in the capture, 0 here).
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_b8460(uint32_t game, uint32_t unit, uint32_t ctx, const D2Seam* e)
{
    // 1.13c D2Game+0xB8460
    auto quit = [&] { e->call(0xDEEE0, 0, game, unit, 0, 0, 0, 1); return 0u; };
    if (!unit) return quit();
    if (d2rd32(unit) == 1 && d2rd32(unit + 0x14)) {
        const uint32_t s = d2rd32(d2rd32(unit + 0x14) + 0x28);
        if (s && d2rd32(s + 0x28)) {
            const uint32_t o = e->call(0xE03A0, d2rd32(s + 0x30), d2rd32(s + 0x28), d2rd32(s + 0x2C));
            if (o && e->call(0xDFA90, o)) return quit();
        }
    }
    if (!(d2rd32(unit + 0xC8) >> 10 & 1)) return quit();
    const uint32_t m = e->call(0xE03A0, d2rd32(unit + 0x94), game, d2rd32(unit + 0x98));
    if (!m || e->call(0xDFA90, m)) return quit();
    uint32_t w[2] = { 0, 0 };   // retail's two zeroed out-words: w[0] at the lower address (4th argument), w[1] (3rd)
    const uint32_t cmd = e->call(0xB7090, 0, 0, 0, 0, 0, game, unit, (uint32_t)(uintptr_t)&w[1], (uint32_t)(uintptr_t)&w[0], 0,
                                 0xB2000000u | 0xB6D30u);
    uint32_t node = 0;
    if (d2rd32(unit) == 1 && d2rd32(unit + 0x14)) {
        const uint32_t s = d2rd32(d2rd32(unit + 0x14) + 0x28);
        const uint32_t head = s ? d2rd32(s + 0x20) : 0;
        if (head) {
            uint32_t n = d2rd32(head);
            while (d2rd32(n + 8) != 10 && n != head) n = d2rd32(n);
            node = d2rd32(n + 8) == 10 ? n : 0;
        }
    }
    if (!node) {
        uint32_t q[5] = { 0, 0, 10, 0, 0 };
        q[3] = e->call(0x1100, 0, unit);
        q[4] = e->call(0x1130, 0, unit);
        e->call(0x21290, game, unit, 0, 0, 0, (uint32_t)(uintptr_t)q);
        e->blk(0x21290, q, 5);
        node = e->call(0x20E00, unit, 0, 0, 0, 10, 0);
    }
    const uint32_t c0 = d2rd32(ctx);
    uint32_t r = e->call(0xB79B0, cmd, unit, 0, 0, 0, game, c0, w[1], w[0], node);
    if (r == 7 || r == 9 || r == 0xE || r == 0xF) r = 2;
    if (!cmd) { e->call(0xF22C0, game, 0, 0, 0, unit, 0xF); return 0; }
    e->call(0xB8070, cmd, unit, r, 0, 0, game, c0, node);
    e->call(0xF20C0, game, 0, 0, unit, 0, 0x19);
    return 0;
}
