// 1.13c D2Game+0x54A20: stack = (game, ctx, rec, outItem, outCount), ret 0x14. Returns 0 on success, 0xD on failure.
// item = 0x54720(ebx = game, esi = rec); null = 0xD. kind = item[0x10]: 0 = set position (0xA8F2/0xA964 from rec[0x18]/[0x1C]) then gate 0xD76A0
// (edx 0x1C3); 1 = equip (d2ik_equip); 6 = needs *outItem, 0xD4A70; else 0xD. On success: if rec[1] then *outItem = item; *outCount = rec[1].
#include "d2game_itemkind.h"
extern "C" uint32_t d2g_54a20(uint32_t game, uint32_t ctx, uint32_t rec, uint32_t outItem, uint32_t outCount, const D2Seam* e)
{
    const uint32_t item = e->call(0x54720, 0, 0, 0, game, rec);
    if (!item) return 0xD;
    const uint32_t kind = d2rd32(item + 0x10);
    bool ok = false;
    if (kind == 0) {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t x = d2rd32(rec + 0x18), y = d2rd32(rec + 0x1C);
        e->call(0xA8F2, 0, 0, 0, 0, 0, item, x);
        e->call(0xA964, 0, 0, 0, 0, 0, item, y);
        const uint32_t ic = d2rd32(item + 0xC);
        const uint32_t r30 = d2ik_1130(e, item), r00 = d2ik_1100(e, item);
        const uint32_t w[8] = { game, ctx, ic, r00, r30, 0, 0, 0 };
        if (!d2ik_gate(e, 0x1C3, w)) return 0xD;
        ok = true;
    } else if (kind == 1) {
        if (!d2ik_equip(e, item, ctx, game)) return 0xD;
        ok = true;
    } else if (kind == 6) {
        if (!d2rd32(outItem)) return 0xD;
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t ec = d2rd32(d2rd32(outItem) + 0xC);
        uint32_t slot = game;
        const uint32_t w[8] = { game, ctx, ec, (uint32_t)(uintptr_t)&slot, 0, 1, 0, 0 };
        if (!d2ik_d4a70(e, d2rd32(item + 0xC), game, w)) return 0xD;
        ok = true;
    }
    if (!ok) return 0xD;
    const uint32_t n = d2rd8(rec + 1);
    if (n) d2wr32(outItem, item);
    d2wr32(outCount, n);
    return 0;
}
