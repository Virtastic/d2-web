// 1.13c D2Game+0xAD410 open-wounds style proc (retail layout). EDX = event kind, stack words: attacker (ESI), target (EDI),
// unused, packed skill/level word (the chance getter gets it whole and shifted right 16), unused; ret 0x14. Returns 1 when the
// proc fired. The unit seed (+0x20 lo, +0x24 hi) advances by seed = lo * 0x6AC690C5 + hi (64-bit), the roll is lo % 100 < chance.
// The damage is the array helper over {9,0x12,0x1B,0x24,0x2D} at the attacker's stat 0xC (at least 1) plus 0x28, divided by 4
// for a player target (a monster-type check halves it again for kind 6 or a flagged monster), applied through the 9-word block
// {attacker, target, 0, 1, 0xC8, 0x4A, -damage, 0x3E, 0}. The callees ride in Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GAD410Env {
    int32_t  (*chance)(uint32_t unit, uint32_t hi, uint32_t packed);          // 0xA802 (ord 10910)
    int32_t  (*getStat)(uint32_t unit, uint32_t stat, uint32_t layer);        // 0xA42A
    int32_t  (*calc)(const uint32_t* arr, int32_t level);                     // 0xAC940 (ECX array, EDX level)
    int32_t  (*isFlagged)(uint32_t unit, uint32_t which);                     // 0x22A80 (EAX unit, one stack word 0xC)
    void     (*apply)(const uint32_t* block);                                 // 0xA00B0 (EAX block)
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_AD410(uint32_t kind, uint32_t a, uint32_t t, uint32_t packed, const D2GAD410Env* e)
{
    if (!a || !t) return 0;
    const int32_t chance = e->chance(a, packed >> 16, packed);
    if (chance <= 0) return 0;
    uint32_t lo, hi;
    std::memcpy(&lo, (void*)(uintptr_t)(a + 0x20), 4); std::memcpy(&hi, (void*)(uintptr_t)(a + 0x24), 4);
    const uint64_t s = (uint64_t)lo * 0x6AC690C5u + hi;
    std::memcpy((void*)(uintptr_t)(a + 0x20), &s, 8);
    if ((int32_t)((uint32_t)s % 100u) >= chance) return 0;
    int32_t lvl = e->getStat(a, 0xC, 0);
    if (lvl < 1) lvl = 1;
    static const uint32_t arr[5] = { 9, 0x12, 0x1B, 0x24, 0x2D };
    int32_t dmg = e->calc(arr, lvl) + 0x28;
    uint32_t type; std::memcpy(&type, (void*)(uintptr_t)t, 4);
    bool halve = false;
    if (type == 0) { dmg = (dmg + ((dmg >> 31) & 3)) >> 2; halve = kind == 6; }
    else if (type == 1) halve = e->isFlagged(t, 0xC) != 0;
    if (halve) dmg = (dmg - (dmg >> 31)) >> 1;
    const uint32_t blk[9] = { a, t, 0, 1, 0xC8, 0x4A, (uint32_t)-dmg, 0x3E, 0 };
    e->apply(blk);
    return 1;
}
