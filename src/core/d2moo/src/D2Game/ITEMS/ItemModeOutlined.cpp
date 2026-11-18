// 1.13c functions that 1.10f inlined into the item-mode code (ItemMode.cpp); split out here as retail has them.
#include <test_seam.h>
#include "ITEMS/ItemMode.h"

#include <D2Inventory.h>
#include <D2Items.h>
#include <D2Skills.h>

// 1.13c D2Game+0xD1A30 (ecx = unit, eax = skill; 1 unless the use-state is 2 (no quantity) or 7 (no level))
int32_t __fastcall ITEMMODE_SkillHasUseState_D1A30(D2UnitStrc* pUnit, D2SkillStrc* pSkill)
{
    const int32_t nUseState = D2_CALL(SKILLS_GetUseState, pUnit, pSkill);
    if (nUseState != SKILLUSTATE_NOQUANTITY && nUseState != SKILLUSTATE_NOLEVEL)
    {
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0xD1A50 (eax = player, edi = main-hand out, ebx = off-hand out; the weapon pick that opens sub_6FC45300)
// Returns what EAX holds at the ret: the unit itself without an inventory, 0 without an off-hand candidate, else the weapon type check.
uintptr_t __fastcall ITEMMODE_GetHandWeapons_D1A50(D2UnitStrc* pUnit, D2UnitStrc** ppMainWeapon, D2UnitStrc** ppRightHandWeapon)
{
    *ppRightHandWeapon = nullptr;
    *ppMainWeapon = nullptr;
    D2InventoryStrc* pInventory = pUnit->pInventory;
    if (!pInventory)
    {
        return (uintptr_t)pUnit;
    }

    *ppMainWeapon = D2_CALL(INVENTORY_GetLeftHandWeapon, pInventory);
    D2UnitStrc* pRight = D2_CALL(INVENTORY_GetItemFromBodyLoc, pInventory, BODYLOC_RARM);
    D2UnitStrc* pLeft = D2_CALL(INVENTORY_GetItemFromBodyLoc, pInventory, BODYLOC_LARM);
    *ppRightHandWeapon = (*ppMainWeapon == pRight) ? pLeft : pRight;
    if (!*ppRightHandWeapon)
    {
        return 0;
    }

    const int32_t bIsWeapon = D2_CALL(ITEMS_CheckItemTypeId, *ppRightHandWeapon, ITEMTYPE_WEAPON);
    if (!bIsWeapon)
    {
        *ppRightHandWeapon = nullptr;
    }
    return (uintptr_t)(uint32_t)bIsWeapon;
}

// 1.13c D2Game+0xD1720 (eax = value; the paired slot of a left/right pair: 4<->5, 6<->7, 0xB<->0xC, anything else 0)
uint32_t __fastcall ITEMMODE_GetPairedSlot_D1720(uint32_t nSlot)
{
    switch (nSlot)
    {
    case 4: return 5;
    case 5: return 4;
    case 6: return 7;
    case 7: return 6;
    case 0xB: return 0xC;
    case 0xC: return 0xB;
    default: return 0;
    }
}

// D2Common ordinal 10267 (1.13c D2Common+0x1DF90) is a bare return; 1.10f dropped the call (ItemMode.cpp has it commented out).
__attribute__((weak)) void __stdcall D2COMMON_10267_Return(D2InventoryStrc* pInventory, int32_t nSlot) {}

// 1.13c D2Game+0xD2540 (eax = start slot, stack = player): the four belt rows from the start slot are packed down:
// every occupied row after the first moves to the next free destination (flags 0x400 and NEWITEM set, NOEQUIP cleared when set).
void __fastcall ITEMMODE_PackBeltRows_D2540(uint32_t nStartSlot, D2UnitStrc* pUnit)
{
    uint32_t nSlot = nStartSlot & 3;
    D2InventoryStrc* pInventory = pUnit->pInventory;
    int32_t nFilled = 0;
    if (nSlot != 0 && nSlot > 2)
    {
        nSlot = 3;
    }

    uint32_t nDestSlot = nSlot;
    for (int32_t nRow = 0; nRow < 4; ++nRow, nSlot += 4)
    {
        D2UnitStrc* pItem = D2_CALL(INVENTORY_GetItemFromBeltSlot, pInventory, (int32_t)nSlot);
        if (!pItem)
        {
            continue;
        }

        if (nFilled != nRow)
        {
            D2_CALL(INVENTORY_PlaceItemInBeltSlot, pInventory, pItem, (int32_t)nDestSlot);
            D2_CALL(ITEMS_SetItemFlag, pItem, IFLAG_UNK1, 1);
            D2_CALL(ITEMS_SetItemFlag, pItem, IFLAG_NEWITEM, 1);
            if (D2_CALL(ITEMS_CheckItemFlag, pItem, IFLAG_NOEQUIP, 0x1314, "..\\Source\\D2Game\\ITEMS\\ItemMode.cpp"))
            {
                D2_CALL(ITEMS_SetItemFlag, pItem, IFLAG_NOEQUIP, 0);
            }
            D2_CALL(UNITS_RefreshInventory, pUnit, 1);
            D2_CALL(INVENTORY_AddItemToTradeInventory, pInventory, pItem);
            D2_CALL(D2COMMON_10267_Return, pInventory, (int32_t)nSlot);
        }
        ++nFilled;
        nDestSlot += 4;
    }
}
