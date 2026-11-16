// 1.13c D2Common+0x50930 (caller 0x52340, the skill-use check): ECX = skill, [esp+4] = unit, ret 4. Does the unit hold what the
// skill's own id asks for. A skill record with bit 0x10 (gdwBitMasks[4]) of byte +8 answers "has an inventory and a positive
// +0x30"; any id but 2, 4 and 5 passes; ids 2, 4 and 5 ask 0x20700 for the hand item, its location and the swap flag, take the
// other hand through 0x4EBF0 when the flag asks, and test the item: id 5 with item type 0x2D, ids 2 and 4 with 0x24C30 (a
// non-zero answer is returned) and then stat 0x7D. 32-bit retail layout through raw reads; a null record faults in retail.
#include "d2common_50930.h"
#include <cstring>

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 4); return v; }
int32_t rd16s(uint32_t a) { int16_t v; std::memcpy(&v, reinterpret_cast<const void*>(uintptr_t(a)), 2); return v; }
uint32_t rd8(uint32_t a) { return *reinterpret_cast<const uint8_t*>(uintptr_t(a)); }
}

// 1.13c D2Common+0x50930
extern "C" uint32_t D2Common_50930_HoldsSkillItem(uint32_t pSkill, uint32_t pUnit, const D2Common50930Env* e)
{
    if (!pUnit || !pSkill) return 0;
    const uint32_t txt = rd32(pSkill);
    const uint32_t inv = rd32(pUnit + 0x60);
    if (txt && (rd8(txt + 8) & 0x10)) return inv ? static_cast<int32_t>(rd32(pSkill + 0x30)) > 0 : 0;
    if (!txt) __builtin_trap();   // retail faults reading the id
    const int32_t id = rd16s(txt);
    // retail's out-words: the item and location locals start undefined, the flag reuses the unit argument's stack slot
    uint32_t item = 0, loc = 0, flag = pUnit, it, chk;
    if (id == 2) {
        if (!inv) return 0;
        if (!e->isPlayerish(pUnit)) {
            if (!e->getHands(inv, &item, &loc, &flag)) return 0;
            chk = flag;
        } else {
            if (!e->getHands(inv, &item, &loc, &flag)) return 0;
            chk = item;
            if (!flag) { item = e->otherHand(loc, inv); chk = item; }
        }
        it = item;
        if (!chk) return 0;
    } else if (id == 4) {
        if (!inv) return 0;
        if (!e->getHands(inv, &item, &loc, &flag)) return 0;
        it = item;
        if (flag) {
            item = e->otherHand(loc, inv);
            it = item;
            if (!it) return 0;
        }
    } else if (id == 5) {
        if (!inv || !e->isPlayerish(pUnit)) return 0;
        if (!e->getHands(inv, &item, &loc, &flag)) return 0;
        if (flag) { item = e->otherHand(loc, inv); if (!item) return 0; }
        return e->isType(item, 0x2D);
    } else {
        return 1;
    }
    if (const uint32_t q = e->quantity(it)) return q;
    return e->getStat(it, 0x7D, 0);
}
