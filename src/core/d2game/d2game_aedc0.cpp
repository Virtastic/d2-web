// 1.13c D2Game+0xAEDC0: ECX = game, EDX = unit, stack item + 4 unused (ret 0x14); gdt is the address of the module's
// sgptDataTables pointer. The 0xAEA40 rule with a per-stat value step first: stats 6/0x4A go through 0xADD2 and stats 8/0x1A
// through 0xADCC (0x1A/0x4A shifted << 8 before), and when the unit's stat 3 (6/0x4A) or 1 (8/0x1A) is positive two rolls on
// the unit's seed (unit+0x20: 0x11D0 with ESI = that stat, 0x1180 with EAX = 100) double the value when the second is below
// half the first. Then << the stat's shift byte, as 0xAEA40.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_aedc0(uint32_t game, uint32_t unit, uint32_t item, uint32_t gdt, const D2Seam* e)
{
    // 1.13c D2Game+0xAEDC0
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
        if (stat < 0) break;
        const uint32_t dt = d2rd32(gdt);
        if (stat >= (int32_t)d2rd32(dt + 0xBD4)) break;
        const uint32_t isc = stat * 0x144u + d2rd32(dt + 0xBCC);
        if (!isc) continue;
        uint32_t v0 = e->call(0xADBA, 0, 0, 0, 0, 0, unit, item, d2rd32(rec + 0xA4 + 4 * i));
        if (stat == 6 || stat == 8 || stat == 0x1A || stat == 0x4A) {
            if (stat == 0x1A || stat == 0x4A) v0 <<= 8;
            const bool mana = stat == 8 || stat == 0x1A;
            v0 = e->call(mana ? 0xADCC : 0xADD2, 0, 0, 0, 0, 0, unit, v0);
            const int32_t base = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, mana ? 1 : 3, 0);
            if (base > 0) {
                const uint32_t half = e->call(0x11D0, 0, unit + 0x20, 0, 0, (uint32_t)base) >> 1;
                if ((int32_t)e->call(0x1180, 100, unit + 0x20) < (int32_t)half) v0 += v0;
            }
        }
        int32_t add = (int32_t)(v0 << (d2rd8(isc + 0x18) & 31));
        if (add <= 0) continue;
        if (stat == 6) life = 1;
        if ((stat == 6 || stat == 0x4A) && unit && d2rd32(unit) == 0)
            add = (int32_t)e->call(0x4EA30, unit, (uint32_t)add, 0, 0, game);
        if (len <= 0) {
            const int32_t old = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, (uint32_t)stat, 0);
            const uint32_t mstat = d2rd16(isc + 0x32);
            if ((int32_t)mstat < (int32_t)d2rd32(d2rd32(gdt) + 0xBD4)) {
                const int32_t maxv = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, mstat, 0);
                if (old + add > maxv) add = maxv - old;
            }
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
            e->call(0xA75A, 0, 0, 0, 0, 0, list, 0);   // stack word 1: the module callback 0x9F7A0 (masked in the capture)
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
        const uint8_t was = (uint8_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 0x160, 0);
        const int32_t cur = (int32_t)e->call(0xA42A, 0, 0, 0, 0, 0, unit, 6, 0) >> 8;
        const int32_t mx = (int32_t)e->call(0xA3A0, 0, 0, 0, 0, 0, unit) >> 8;
        const uint8_t fill = (mx > 0 && cur < mx) ? (uint8_t)((cur << 7) / mx) : 0x80;
        const int32_t d = (int32_t)was - (int32_t)fill;
        if ((d < 0 ? -d : d) > 4) {
            e->call(0x7A260, 0, 0, 0, 0, unit, (item & ~0xFFu) | fill);   // the item argument's slot with its low byte replaced
            e->call(0xA472, 0, 0, 0, 0, 0, unit, 0x160, fill, 0);
        }
    }
    return result;
}
