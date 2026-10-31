// D2Game+0x4E230 -- one fifth of a non-positive resist-group value once the unit's stat reaches 100.
// edi = stat, ebx = value, esi = unit. Only stats 0x24 0x25 0x27 0x29 0x2b 0x2d take the rule; every other stat
// returns value. The rule: value <= 0, the unit is not a player (null counts as not a player), hirelingType(unit) == 0
// and baseStat(unit, stat, 0) >= 100 -> value / 5 (signed).
#include <cstdint>
#include <cstring>

// 1.13c D2Game+0x4E230
extern "C" int32_t d2game_4e230_impl(uint32_t stat, int32_t value, void* unit,
    int32_t (*hirelingType)(void* unit), int32_t (*baseStat)(void* unit, uint32_t stat, uint32_t layer))
{
    switch (stat) {
    case 0x24: case 0x25: case 0x27: case 0x29: case 0x2b: case 0x2d: break;
    default: return value;
    }
    if (value > 0) return value;
    if (unit != nullptr) { uint32_t t; std::memcpy(&t, unit, 4); if (t == 0) return value; }
    if (hirelingType(unit) != 0) return value;
    if (baseStat(unit, stat, 0) < 100) return value;
    return value / 5;
}
