// 1.13c D2Game+0xAE6D0: ECX = game, EDX = unit, stack item + four unused words (ret 0x14). Applies a
// consumable's timed stat bonuses: the item's record (0xA53E on the item's class) names a duration stat (rec+0xB0) and an
// item type (rec+0x98) and up to three stat ids (rec+0x9E, short each) with their value stats (rec+0xA4, dword each).
// A positive duration adds to (or opens) a timed stat list keyed by the item type; zero adds straight to the base stat,
// clamped to the stat's max. Stat 6 (life) also refreshes the player's life orb. Returns 1 when any stat was applied.
// gdt is the address of the module's sgptDataTables pointer.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_ae6d0(uint32_t game, uint32_t unit, uint32_t item, uint32_t gdt, const D2Seam* e)
{
    // 1.13c D2Game+0xAE6D0
    const uint32_t rec = e->call(0xA53E, 0, 0, 0, 0, 0, item ? d2rd32(item + 4) : 0xFFFFFFFFu);
    if (!rec) return 0;
    const int32_t len = (int32_t)e->call(0xADBA, 0, 0, 0, 0, 0, unit, item, d2rd32(rec + 0xB0));
    const int32_t itype = (int16_t)d2rd16(rec + 0x98);
    uint32_t list = 0;
    int32_t remaining = 0;
    if (len > 0) {
        if (itype <= 0 || itype >= (int32_t)d2rd32(d2rd32(gdt) + 0xC4)) return 0;
        list = e->call(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)itype);
        if (list) remaining = (int32_t)(e->call(0xAB74, 0, 0, 0, 0, 0, list) - d2rd32(game + 0xA8));
    }
    uint32_t result = 0, life = 0;
    for (int32_t i = 0; i < 3; i++) {
        const int32_t stat = (int16_t)d2rd16(rec + 0x9E + 2 * i);
        if (stat < 0) continue;
        const uint32_t dt = d2rd32(gdt);
        const int32_t nstat = (int32_t)d2rd32(dt + 0xBD4);
        if (stat >= nstat) continue;
        const uint32_t isc = stat * 0x144u + d2rd32(dt + 0xBCC);
        if (!isc) continue;
        if ((int32_t)d2rd16(isc + 0x32) >= nstat) continue;
        const int32_t value = (int32_t)e->call(0xADBA, 0, 0, 0, 0, 0, unit, item, d2rd32(rec + 0xA4 + 4 * i));
        if (value <= 0) continue;
        const int32_t maxv = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, d2rd16(isc + 0x32), 0);
        int32_t add = (int32_t)e->call(0x14D0, (uint32_t)value, 100, (uint32_t)maxv);
        if (value == 100) add = maxv;
        if (stat == 6) life = 1;
        if ((stat == 6 || stat == 0x4A) && unit && d2rd32(unit) == 0)
            add = (int32_t)e->call(0x4EA30, unit, (uint32_t)add, 0, 0, game);
        if (len <= 0) {
            const int32_t old = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, (uint32_t)stat, 0);
            if (old + add > maxv) add = maxv - old;
            e->call(0xA4D2, 0, 0, 0, 0, 0, unit, (uint32_t)stat, (uint32_t)add, 0);
            result = 1;
            continue;
        }
        const uint32_t expire = d2rd32(game + 0xA8) + (uint32_t)remaining + (uint32_t)len;
        if (!list) {
            list = e->call(0xA6A0, 0, 0, 0, 0, 0, d2rd32(game + 0x1C), 2, expire,
                           unit ? d2rd32(unit) : 6, unit ? d2rd32(unit + 0xC) : 0xFFFFFFFFu);
            if (!list) continue;
            e->call(0xA5B6, 0, 0, 0, 0, 0, list, (uint32_t)itype);
            e->call(0xA3CA, 0, 0, 0, 0, 0, unit, (uint32_t)itype, 1);
            if (!unit || d2rd32(unit) != 0) {
                if (unit)
                    for (uint32_t n = d2rd32(unit + 0xDC); n; ) {
                        const uint32_t next = d2rd32(n + 0x24);
                        if (d2rd8(n) == 3) { e->call(0x8E6F0, n); e->edi(0x8E6F0, game); }
                        n = next;
                    }
                e->call(0x8EA80, 0, game, unit, 0, 0, 3, d2rd32(game + 0xA8) + 1, 0, 0, 0);
            }
            // stack word 1 is the module callback 0x9F7A0 (masked in the capture)
            e->call(0xA75A, 0, 0, 0, 0, 0, list, 0);
            e->call(0xA60A, 0, 0, 0, 0, 0, unit, list, 1);
        }
        e->call(0xA7C6, 0, 0, 0, 0, 0, list, expire);
        const int32_t v = (int32_t)e->call(0xA712, 0, 0, 0, 0, 0, list, (uint32_t)stat, 0) * remaining + add;
        if (i == 0) e->call(0x8EA80, 0, game, unit, 0, 0, 0xC, expire, 0, 0, 0);
        const int32_t span = remaining + len;
        if (span > 0) e->call(0xA6E2, 0, 0, 0, 0, 0, list, (uint32_t)stat, (uint32_t)(v / span), 0);
        result = 1;
    }
    if (life && unit && d2rd32(unit) == 0) {
        const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 6, 0) >> 8;
        const int32_t mx = (int32_t)e->call(0xA3A0, 0, 0, 0, 0, 0, unit) >> 8;
        const uint8_t bl = (mx > 0 && cur < mx) ? (uint8_t)((cur << 7) / mx) : 0x80;
        // the item argument's stack slot, its low byte overwritten, is the pushed word
        e->call(0x7A260, 0, 0, 0, 0, unit, (item & ~0xFFu) | bl);
        e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x160, bl, 0);
    }
    return result;
}
