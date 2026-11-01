// 1.13c D2Game+0x48950 -- copy a hireling's base stats back onto it (ECX = game, EAX = player, ret).
// Written from the 1.13c disassembly as a rule over the seam environment (d2game_gp.h); T = the sgptDataTables block.
// pd = 0xA4C0(player); none, a hireling table count (T +0xBF0) of 7 or less, no pd +0x44 list, or no first node ([[pd +0x44]] +0x54): return.
// Walk the nodes by +0x14 to the first whose byte 0 has bit 0 clear (none: return); guid = node +4. The merc is the unit in game +0x1320
// bucket (guid & 0x7F), chained by +0xE4, whose +0xC is the guid (none: return). Then 0x7A3A0(ESI = merc, BL = stat; value) for
// 0xC (0xA42A), 0, 2, 7 (0xA3E2), 6 (0xA42A), 0x1F (0xA3E2), 0xD, 0x1E (0xA42A), 0x15 = 0xA3E2 of 0x17 + 0x15, 0x16 = 0x18 + 0x16,
// and 0x27, 0x29, 0x2B, 0x2D (0xA3E2), each read as (merc, stat, 0).
#include "../fn/d2game_gp.h"
namespace d2g {
static uint32_t bp48950(GE& e, uint32_t v) { return (v >> 24) == 0xB1 ? e.B((int)(v >> 16 & 0xFF)) + (v & 0xFFFF) : v; }
void f48950(GE& e, uint32_t T, uint32_t game, uint32_t player)
{
    const uint32_t pd = bp48950(e, e.h(0xA4C0, 0, 0, 0, 0, 0, player));
    if (!pd || (int32_t)e.rd32(T + 0xBF0) <= 7) return;
    const uint32_t p = e.rd32(pd + 0x44);
    if (!p) return;
    uint32_t node = e.rd32(e.rd32(p) + 0x54);
    if (!node) return;
    while (e.rd8(node) & 1) { node = e.rd32(node + 0x14); if (!node) return; }
    const uint32_t guid = e.rd32(node + 4);
    uint32_t m = e.rd32(game + 0x1320 + (guid & 0x7F) * 4);
    while (m && e.rd32(m + 0xC) != guid) m = e.rd32(m + 0xE4);
    if (!m) return;
    auto set = [&](uint32_t st, uint32_t v) { e.h(0x7A3A0, 0, 0, 0, st, m, v); };
    auto g42a = [&](uint32_t st) { return e.h(0xA42A, 0, 0, 0, 0, 0, m, st, 0); };
    auto g3e2 = [&](uint32_t st) { return e.h(0xA3E2, 0, 0, 0, 0, 0, m, st, 0); };
    set(0xC, g42a(0xC));
    set(0, g3e2(0));
    set(2, g3e2(2));
    set(7, g3e2(7));
    set(6, g42a(6));
    set(0x1F, g3e2(0x1F));
    set(0xD, g42a(0xD));
    set(0x1E, g42a(0x1E));
    { const uint32_t a = g3e2(0x17); set(0x15, a + g3e2(0x15)); }
    { const uint32_t a = g3e2(0x18); set(0x16, a + g3e2(0x16)); }
    set(0x27, g3e2(0x27));
    set(0x29, g3e2(0x29));
    set(0x2B, g3e2(0x2B));
    set(0x2D, g3e2(0x2D));
}
}
