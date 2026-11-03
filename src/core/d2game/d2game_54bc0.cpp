// 1.13c D2Game+0x54BC0: ecx = game, eax = ctx, stack = (rec, outItem, outCount), ret 0xC. Returns 0 on success, 0xC on failure.
// *outCount = 0 first (when non-null). item = 0x54720(ebx = game, esi = rec); null or kind > 6 = 0xC. Kind 0: 0xA85C(item, &a, &b, "PlrSave.cpp", 0xE2)
// reads the item's grid cell into b, then 0xA8F2/0xA964 from rec[0x18]/[0x1C], gate 0xD76A0 (edx 0xEB); 1 = 0xD60D0; 2 = 0xA8F2/0xA964 + 0xD4E80;
// 4 = 0xAA54 + 0xD1650; 6 = needs *outItem, 0xD4A70; 3 and 5 = 0xC. Success: if rec[1] then *outItem = item; if outCount then *outCount = rec[1].
#include "d2game_itemkind.h"
extern "C" uint32_t d2g_54bc0(uint32_t game, uint32_t ctx, uint32_t rec, uint32_t outItem, uint32_t outCount, const D2Seam* e)
{
    if (outCount) d2wr32(outCount, 0);
    const uint32_t item = e->call(0x54720, 0, 0, 0, game, rec);
    if (!item) return 0xC;
    const uint32_t kind = d2rd32(item + 0x10);
    if (kind > 6) return 0xC;
    uint32_t slot = rec;                                  // the stack slot of the first argument: retail hands out its address
    uint32_t r = 0;
    switch (kind) {
    case 0: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        uint32_t bufA = 0, cell = 0, bufB = d2rd32(rec + 0x18), bufC = d2rd32(rec + 0x1C);   // A85C's two outs are separate locals; A8F2/A964 read the saved rec words
        e->call(0xA85C, 0, 0, 0, 0, 0, item, (uint32_t)(uintptr_t)&bufA, (uint32_t)(uintptr_t)&cell, 0xB2000000u | 0xFC138u, 0xE2);
        e->call(0xA8F2, 0, 0, 0, 0, 0, item, bufB);
        e->call(0xA964, 0, 0, 0, 0, 0, item, bufC);
        const uint32_t ic = d2rd32(item + 0xC);
        const uint32_t r30 = d2ik_1130(e, item), r00 = d2ik_1100(e, item);
        const uint32_t w[8] = { game, ctx, ic, r00, r30, 0, 1, 0 };
        r = d2ik_gate(e, 0xEB, w);
        break; }
    case 1: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        r = e->call(0xD60D0, d2rd32(item + 0xC), ctx, 0, 0, 0, game, d2rd8(rec), 1, (uint32_t)(uintptr_t)&slot);
        break; }
    case 2: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        e->call(0xA8F2, 0, 0, 0, 0, 0, item, d2rd32(rec + 0x18));
        e->call(0xA964, 0, 0, 0, 0, 0, item, d2rd32(rec + 0x1C));
        r = e->call(0xD4E80, d2rd32(item + 0xC), ctx, game, 0, 0, d2rd32(rec + 0x18), 0, (uint32_t)(uintptr_t)&slot);
        break; }
    case 4: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t x = e->call(0xAA54, 0, 0, 0, 0, 0, d2rd32(ctx + 0x60), item);
        r = e->call(0xD1650, x);
        break; }
    case 6: {
        e->call(0xA4A2, 0, 0, 0, 0, 0, item, 4);
        const uint32_t cur = d2rd32(outItem);
        if (!cur) return 0xC;
        const uint32_t ic = d2rd32(item + 0xC);
        const uint32_t w[8] = { game, ctx, d2rd32(cur + 0xC), (uint32_t)(uintptr_t)&slot, 0, 1, 0, 0 };
        r = d2ik_d4a70(e, ic, ic, w);
        break; }
    default: return 0xC;
    }
    if (!r) return 0xC;
    const uint32_t n = d2rd8(rec + 1);
    if (n) d2wr32(outItem, item);
    if (outCount) d2wr32(outCount, n);
    return 0;
}
