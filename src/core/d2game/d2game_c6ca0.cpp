// D2Game+0xC6CA0 -- ACT2Q2_UpdateHoradricItemCounts: EBX = game, EDI = player unit, plain ret. Written from the 1.13c disassembly; callees are the D2Seam.
// Quest list: game+0x10F4 points at a head pointer, nodes chain through +0xF4, kind at +0. The type-9 node's data block (node+0x18) gets
// +0x2A := 1, +0x2C := unit id (or -1), +0x18 and +0x24 decremented, +0x1C incremented. Then 0xA4C0 (unit: player data), 0x34C00
// (EAX = unit, stack game), 0xA57A (flags from player data + difficulty*4 + 0x10, 10, 11), and a tail jump into 0xC6920 (EAX = game), included here:
// the type-0xB node with +9 clear runs 0xC5D10 (EAX = data, stack node); with +9 set it runs 0x35EB0 (EAX = 1, ESI = node) when +0xC is clear, and
// when +0xB is clear sets +0x14 := 0, +0xB := 1 and 0xDFCC0 (EAX 0, EBX callback 0xC5030, EDI = node+4, stack 0).
#include "d2game_seam4e.h"
static uint32_t c6ca0_find(uint32_t game, uint32_t kind)
{
    const uint32_t list = d2rd32(game + 0x10F4);
    if (!list) return 0;
    uint32_t n = d2rd32(list);
    while (n && d2rd32(n) != kind) n = d2rd32(n + 0xF4);
    return n;
}
// 1.13c D2Game+0xC6CA0
extern "C" void d2game_c6ca0(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t n9 = c6ca0_find(game, 9);
    if (n9) {
        const uint32_t d = d2rd32(n9 + 0x18);
        d2wr8(d + 0x2A, 1);
        const uint32_t id = unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu;
        const uint32_t a = d2rd32(d + 0x18), b = d2rd32(d + 0x24), c = d2rd32(d + 0x1C);
        d2wr32(d + 0x2C, id);
        d2wr32(d + 0x18, a - 1);
        d2wr32(d + 0x24, b - 1);
        d2wr32(d + 0x1C, c + 1);
    }
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    const uint32_t flags = d2rd32(pd + d2rd8(game + 0x6D) * 4 + 0x10);
    e->call(0x34C00, unit, 0, 0, 0, 0, game);
    e->call(0xA57A, 0, 0, 0, 0, 0, flags, 0xA, 0xB);
    // tail 0xC6920
    const uint32_t nb = c6ca0_find(game, 0xB);
    if (!nb) return;
    if (!d2rd8(nb + 9)) { e->call(0xC5D10, d2rd32(nb + 0x18), 0, 0, 0, 0, nb); return; }
    if (!d2rd8(nb + 0xC)) e->call(0x35EB0, 1, 0, 0, 0, nb);
    if (!d2rd8(nb + 0xB)) {
        d2wr8(nb + 0x14, 0);
        d2wr8(nb + 0xB, 1);
        e->call(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC5030, d2rd32(nb + 4), 0);
    }
}
