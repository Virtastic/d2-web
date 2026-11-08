// 1.13c D2Game+0xADDC0 (sub_ADDC0): item-property event callback 0x20, chance to cast on strike. No register arguments; stack (ret 0x14) =
// attacker, unit, damage ptr, packed, arg5 (unused). Written from the 1.13c disassembly as a rule over the seam environment (see d2game_294a0.cpp).
// attacker null: 0. chance = 0xA802(attacker, packed >> 16, packed) (Ordinal 10910); chance <= 0 (signed): 0. id = (packed & 0xFFFF) >> (tables+0xC6C & 31),
// level = tables+0xC70 & packed & 0xFFFF. A damage pointer whose byte 0 lacks 0x20: 0. Seed step on attacker+0x20/+0x24 (lo * 0x6AC690C5 + hi, 64 bit);
// the new lo % 100 must be (signed) below the chance, else 0. Row = 0x1420(EAX = id), none: 0. gdwBitMasks[0x14] & row[7] set: with a unit
// 0xF17E0(EAX = unit, ECX = unit; id, level, 0); bit clear: with a unit 0xF17E0(EAX = unit, ECX = attacker; id, level, 1), without one
// x = 0xAAD8(attacker+0x2C), y = 0xAAC6(attacker+0x2C), 0xF1730(EAX = attacker, EBX = x; id, level, y, 1). Result 1 once the row is found.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t faddc0(GE& e, uint32_t T, uint32_t bm, uint32_t att, uint32_t unit, uint32_t dmg, uint32_t packed)
{
    if (!att) return 0;
    const int32_t chance = (int32_t)e.h(0xA802, 0, 0, 0, 0, 0, att, packed >> 16, packed);
    if (chance < 1) return 0;
    const uint32_t lo = packed & 0xFFFF, level = e.rd32(T + 0xC70) & lo, id = (uint32_t)((int32_t)lo >> (e.rd32(T + 0xC6C) & 31));
    if (dmg && !(e.rd8(dmg) & 0x20)) return 0;
    const uint64_t s = (uint64_t)e.rd32(att + 0x20) * 0x6AC690C5ull + e.rd32(att + 0x24);
    e.wr32(att + 0x20, (uint32_t)s); e.wr32(att + 0x24, (uint32_t)(s >> 32));
    if ((int32_t)((uint32_t)s % 100) >= chance) return 0;
    const uint32_t row = e.h(0x1420, id);
    if (!row) return 0;
    if (e.rd8(bm + 0x14) & e.rd8(row + 7)) {
        if (unit) e.h(0xF17E0, unit, unit, 0, 0, 0, id, level, 0);
        return 1;
    }
    if (unit) { e.h(0xF17E0, unit, att, 0, 0, 0, id, level, 1); return 1; }
    const uint32_t path = e.rd32(att + 0x2C);
    const uint32_t x = e.h(0xAAD8, 0, 0, 0, 0, 0, path), y = e.h(0xAAC6, 0, 0, 0, 0, 0, path);
    e.h(0xF1730, att, 0, 0, x, 0, id, level, y, 1);
    return 1;
}
}
