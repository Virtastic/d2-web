// Decompile-derived, 1.13c-only. Monster elemental enchant (poison variant); the cold variant in d2game_22490.cpp shares the body.
// Seams: stat get (#10973), stat add (#10551), the resist follow-up (D2Game+0x21BE0). The hireling-constant lookup (D2Game+0x21FB0)
// is a leaf over the data tables and runs inline.
#include "d2game_enchant_22320.h"
#include <cstring>

static uint32_t r32(uint32_t base, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(base + o), 4); return v; }
static uint8_t r8(uint32_t base, uint32_t o) { return *(const uint8_t*)(uintptr_t)(base + o); }

// D2Game+0x21FB0 (EAX = index): the +0x1C word of the 0x20-byte row at tables+0xC50, 0 when out of range.
static int32_t constant21fb0(uint32_t tables, int32_t idx)
{
    if (idx < 0 || idx >= (int32_t)r32(tables, 0xc54)) return 0;
    const uint32_t row = r32(tables, 0xc50) + (uint32_t)idx * 0x20u;
    return row ? (int32_t)r32(row, 0x1c) : 0;
}

extern "C" void d2game_enchant_body_22320(uint32_t unit, uint32_t modifier, uint32_t unique, uint32_t firstStat, bool poison,
                                          const D2GameEnchantEnv* env)
{
    if (!unit || r32(unit, 0) != 1) return;
    const uint32_t g = r32(unit, 0x80);
    if (!g) return;
    int32_t diff = r8(g, 0x6d); if (diff > 2) diff = 2;
    const int32_t gt = (r8(g, 0x6a) || r32(g, 0x74)) ? 1 : 0;
    int32_t lv = env->getStat(unit, 0xc, 0);
    const uint32_t t = env->tables;
    const int32_t count = (int32_t)r32(t, 0xb74);
    if (lv <= 1) lv = 1;
    if (lv >= count - 1) lv = count - 1;
    if (lv < 0 || lv >= count) return;
    const uint32_t row = r32(t, 0xb70) + (uint32_t)lv * 0x78u;
    if (!row) return;
    const int32_t dmg = (int32_t)r32(row, (uint32_t)(diff + (gt + 6) * 3) * 4u);
    const int32_t lo = (int32_t)((uint32_t)constant21fb0(t, diff + (unique ? 0x1c : 0x10)) * (uint32_t)dmg) / 100;
    const int32_t hi = (int32_t)((uint32_t)constant21fb0(t, diff + (unique ? 0x1f : 0x13)) * (uint32_t)dmg) / 100;
    env->addStat(unit, firstStat, lo, 0);
    env->addStat(unit, firstStat + 1, hi, 0);
    const uint32_t len = (uint32_t)lv * 5u + (poison ? 0x96u : 0x64u);
    env->addStat(unit, firstStat + 2, (int32_t)(poison ? len * 2u : len), 0);
    env->resist(unit, modifier, unique);
}

// 1.13c D2Game+0x22320
// ECX = unit, EDX = modifier, stack unique (ret 4): poison min/max (stats 0x39/0x3A) from the level row and the constant table, length 0x3B.
extern "C" void d2game_enchant_22320(uint32_t unit, uint32_t modifier, uint32_t unique, const D2GameEnchantEnv* env)
{
    d2game_enchant_body_22320(unit, modifier, unique, 0x39, true, env);
}
