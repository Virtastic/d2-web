// 1.13c D2Common+0x29F30 -- item default defense / damage raise (ESI = item, [esp+4] = stat, ret 4), written from the
// disassembly with its callees behind an env so the FN probe can replay retail's hooked call sequence.
// The vendored ITEMS_SetDefenseOrDamage (D2MOO) works on the same records; retail reads the base stat 160/159 before the
// throwable test. Row offsets: +0x80 code, +0xd0 maxac, +0xfe..+0x103 damage bytes.
#include <cstdint>
#include <cstdlib>
#include "d2common_29f30.h"

// 1.13c D2Common+0x29F30
extern "C" void D2Common_29F30_ItemDefenseOrDamage(uintptr_t item, uint32_t stat, const D2Common29F30Env* e)
{
    if (!item || e->rd32(item) != 4) abort();   // asserts 0x61b / 0x61c, then _exit(-1)
    const uintptr_t txt = e->itemTxt(e->rd32(item + 4));
    if (!txt) abort();                           // assert 0x61f
    const int32_t idx = e->codeIndex(e->rd32(txt + 0x80));
    if (idx < 0) return;
    const uintptr_t row = e->weaponArmorRow(idx);
    if (!row) return;
    if (stat - 0x10u > 0xf) return;
    auto raise = [&](int32_t st, uint32_t v) {
        if (e->getBase(item, st) < (int32_t)v) e->setStat(item, st, (int32_t)v);
    };
    switch (stat) {
    case 0x10: case 0x1f: {
        if (!e->typeIs(item, 0x32)) return;
        const int32_t maxac = (int32_t)e->rd32(row + 0xd0);
        if (!maxac) return;
        int32_t v = e->getBase(item, 0x1f) + 1;
        if (v <= maxac) v = maxac + 1;
        e->setStat(item, 0x1f, v);
        return;
    }
    case 0x11: case 0x16:
        if (!e->typeIs(item, 0x2d)) return;
        if (e->rd8(row + 0xff)) raise(0x16, e->rd8(row + 0xff));
        if (e->rd8(row + 0x103)) raise(0x18, e->rd8(row + 0x103));
        {
            const int32_t cur = e->getBase(item, 0xa0);
            if (e->isThrowable(item) && e->rd8(row + 0x101) && cur < (int32_t)e->rd8(row + 0x101))
                e->setStat(item, 0xa0, (int32_t)e->rd8(row + 0x101));
        }
        return;
    case 0x12: case 0x15:
        if (!e->typeIs(item, 0x2d)) return;
        if (e->rd8(row + 0xfe)) raise(0x15, e->rd8(row + 0xfe));
        if (e->rd8(row + 0x102)) raise(0x17, e->rd8(row + 0x102));
        {
            const int32_t cur = e->getBase(item, 0x9f);
            if (e->isThrowable(item) && e->rd8(row + 0x100) && cur < (int32_t)e->rd8(row + 0x100))
                e->setStat(item, 0x9f, (int32_t)e->rd8(row + 0x100));
        }
        return;
    default: return;
    }
}

// Live path: the callees run for real on the carried graph, which is the vendored routine on the same records.
#include "D2Items.h"
extern "C" void D2Common_29F30_ItemDefenseOrDamageLive(D2UnitStrc* item, int32_t stat)
{
    ITEMS_SetDefenseOrDamage(item, stat);
}
