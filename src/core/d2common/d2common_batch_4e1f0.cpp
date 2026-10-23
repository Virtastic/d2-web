// Batch b146: SKILLS_SetCharges (0x4e1f0), INVENTORY_GetUseableItemFromBeltSlot (0x20010).
#include <cstdint>
typedef uintptr_t P;

// 1.13c D2Common+0x4e1f0: skill-list entry +0x34 == -1 (no charge slot) -> 0; else +0x3c = 1, +0x38 = n, return 1.
// A null entry is fatal in retail (assert text + _exit(-1)); fatal(0x658) stands for that path.
uint32_t D2Common_4E1F0_SetCharges(P pSkill, uint32_t n, void (*fatal)(uint32_t))
{
    if (!pSkill) { fatal(0x658); return 0; }
    if (*(int32_t*)(pSkill + 0x34) == -1) return 0;
    *(uint32_t*)(pSkill + 0x3c) = 1;
    *(uint32_t*)(pSkill + 0x38) = n;
    return 1;
}

// 1.13c D2Common+0x20010: inventory (magic 0x1020304) + grid(&DAT_6fded390) present, item unit type 4,
// beltable (0x24ba0), 1x1, itemstxt +0x11d == 1 and +0x130 == 1 -> *out = belt item at slot (0x1e5b0), return 1.
uint32_t D2Common_20010_GetUseableItemFromBeltSlot(
    P pInv, P pItem, int32_t slot, uint32_t* out,
    P (*getGrid)(P), uint8_t (*beltable)(P), void (*getDim)(P, uint8_t*, uint8_t*),
    P (*itemsTxt)(uint32_t), uint32_t (*beltItem)(P, int32_t))
{
    if (!pInv || *(int32_t*)pInv != 0x1020304) return 0;
    if (!getGrid(0xded390 /*grid table ref*/)) return 0;
    if (!pItem || *(int32_t*)pItem != 4) return 0;
    if (!beltable(pItem)) return 0;
    uint8_t w, h;
    getDim(pItem, &w, &h);
    if (w != 1 || h != 1) return 0;
    P t = itemsTxt(*(uint32_t*)(pItem + 4));
    if (!t || *(int8_t*)(t + 0x11d) != 1 || *(int8_t*)(t + 0x130) != 1) return 0;
    *out = beltItem(pInv, slot);
    return 1;
}
