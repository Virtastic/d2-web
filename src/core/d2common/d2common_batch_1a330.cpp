// 1.13c D2Common batch 5: +0x1A330, +0x2A210, +0x22880, +0x230A0, +0x27090, +0x3CD80,
// +0x4D870, +0x53020.  These are link-safe naming implementations: the parts pinned by the
// 1.13c decompile are written out; helpers that are separate, still-UNMAPPED rows are named in
// ponytail notes and are not called (their symbols do not exist yet).
#include <cstddef>
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/ItemsTbls.h"
#include "DataTbls/MonsterTbls.h"

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
}
// retail fatal: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1). Seams are weak; skip if unlinked.
[[noreturn]] static void d2_fatal(uint32_t code)
{
    if (Ordinal_10265 && Ordinal_10024)
        Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
    __builtin_trap();
}

// 1.13c D2Common+0x1A330 (#10065): monster chain-id pair lookup.
// pUnit must be a UNIT_MONSTER (dwUnitType 0, +0x00); dwClassId (+0x04) indexes monsters.txt
// (stride 0x1A8) and its byte +0x4B (nChainId) must be <= 3 (else retail fatals with 0x2C8).
// An out-of-range class id reads chain id 0. The two outs are the dword pair of the module table
// at 0x6FDE4E78 (four entries, second half at 0x6FDE4E7C) for that chain id.
// ponytail: that table is module data whose values are not yet extracted; the pair is zero for now.
uint32_t D2Common_1A330_MonsterChainPair(const void* pUnit, uint32_t* pOutChain, uint32_t* pOutMax)
{
    static_assert(offsetof(D2MonStatsTxt, nChainId) == 0x4B, "monsters.txt nChainId");
    static const uint32_t kChainPair[4][2] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };
    if (pOutChain == nullptr || pOutMax == nullptr)
        return 0;                                   // retail has no check (would fault)
    const uint32_t* u = static_cast<const uint32_t*>(pUnit);
    if (pUnit == nullptr || u[0] != 1)
        d2_fatal(700);                              // not a monster unit
    const D2MonStatsTxt* pRow = DATATBLS_GetMonStatsTxtRecord(static_cast<int32_t>(u[1]));
    uint32_t nChain = pRow != nullptr ? pRow->nChainId : 0;  // out of range -> 0, as retail
    if (nChain > 3)
        d2_fatal(0x2C8);
    *pOutChain = kChainPair[nChain][0];
    *pOutMax   = kChainPair[nChain][1];
    return 1;
}

// 1.13c D2Common+0x2A210 (#10071): inventory durability/quantity check and repair-cost sum.
// pUnit+0x60 is the inventory, whose first dword must be the inventory magic 0x1020304; slots are
// walked at byte offsets 0..0x34 step 4. For each item retail checks durability vs items.txt
// (ItemStatCost record 72 / +0x5B20, count >= 0x49) and quantity vs record 70 (+0x5898, count
// >= 0x47), then sums 0x28E80(item, nArg4, pArg5, nArg3, 3) for flagged items and calls
// pfn(pUnit) once per counted item.
// ponytail: every per-item helper (0x1E1B0, 0x24FF0, 0x38A80, 0x24F00, 0x26D40, 0x38FF0,
// 0x260F0, 0x27090, 0x28E80) is a separate, still-UNMAPPED row; this slice pins the gate and walk.
int32_t D2Common_2A210_InventoryRepairCount(void* pArg1, void* pUnit, int32_t nArg3, int32_t nArg4,
                                           uint32_t* pArg5, void (*pfn)(void*))
{
    (void)pArg1; (void)nArg3; (void)nArg4; (void)pArg5; (void)pfn;
    if (pUnit == nullptr)
        return 0;
    const int32_t* pInventory = *reinterpret_cast<const int32_t**>(static_cast<uint8_t*>(pUnit) + 0x60);
    if (pInventory == nullptr || pInventory[0] != 0x1020304)
        return 0;
    int32_t nSum = 0;
    for (int32_t nOffset = 0; nOffset < 0x34; nOffset += 4)
    {
        // ponytail: per-slot item pointer and the checks above land with 0x1E1B0 and 0x27090.
        (void)nOffset;
    }
    return nSum;
}

// 1.13c D2Common+0x22880 (#10081): items.txt row nItemId, byte +0x13E (nGemApplyType).
// Out of range is fatal in retail (_exit).
uint32_t D2Common_22880_ItemGemApplyType(uint32_t nItemId)
{
    static_assert(offsetof(D2ItemsTxt, nGemApplyType) == 0x13E, "items.txt nGemApplyType");
    const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
    if (!pRow)
        d2_fatal(0xF29);   // retail: 0x1EF if the table is unloaded, else 0xF29; the getter cannot tell them apart
    return pRow->nGemApplyType;
}

// 1.13c D2Common+0x230A0 (#10082): items.txt row nItemId, int16 at +0x11E (wType[0]), sign-extended.
// Out of range is fatal in retail.
int32_t D2Common_230A0_ItemTypeFromItemId(uint32_t nItemId)
{
    static_assert(offsetof(D2ItemsTxt, wType) == 0x11E, "items.txt wType");
    const D2ItemsTxt* pRow = DATATBLS_GetItemsTxtRecord(static_cast<int32_t>(nItemId));
    if (!pRow)
        d2_fatal(0x68E);   // retail: 0x1EF if the table is unloaded, else 0x68E
    return static_cast<int16_t>(pRow->wType[0]);
}

// 1.13c D2Common+0x27090 (#10087): item usability gate.
// True only for a UNIT_ITEM (type 4, +0x00) that has item data (+0x14) whose flags dword (+0x18)
// carries 0x10 and not 0x400000; retail then runs 0x24FF0 (fails if it sets a non-null out) and
// 0x230F0, requires 0x51300's record with byte +8 set, and finishes with 0x24F00 ? true :
// 0x26D40 (ITEMS_HasDurability).
// ponytail: 0x24FF0 / 0x230F0 / 0x51300 / 0x24F00 / 0x26D40 are still-UNMAPPED rows; only the
// flag gate is pinned here, the rest of the answer is not.
uint32_t D2Common_27090_ItemCheck(const void* pUnit)
{
    if (pUnit == nullptr)
        return 0;
    const uint32_t* u = static_cast<const uint32_t*>(pUnit);
    if (u[0] != 4)                                  // UNIT_ITEM
        return 0;
    const uint8_t* pItemData = *reinterpret_cast<const uint8_t* const*>(static_cast<const uint8_t*>(pUnit) + 0x14);
    if (pItemData == nullptr)
        return 0;
    const uint32_t dwFlags = *reinterpret_cast<const uint32_t*>(pItemData + 0x18);
    if ((dwFlags & 0x10) == 0 || (dwFlags & 0x400000) != 0)
        return 0;
    return 1; // ponytail: remaining checks live in the unported rows above.
}

// 1.13c D2Common+0x3CD80 (#10093): if pUnit is non-null, calls 0x455B0(a2, *(int*)(pUnit+0x10), a2).
// ponytail: 0x455B0 is a still-UNMAPPED row; the call is recorded here but not linked.
void D2Common_3CD80_Forward(const void* pUnit, uint32_t nArg)
{
    (void)pUnit; (void)nArg;
}

// 1.13c D2Common+0x4D870 (#10078): thin forwarder to 0x4D140 with the argument list
// (a1, a2, NULL, a3, a4, a4, a5, a6, 1).
// ponytail: 0x4D140 is a still-UNMAPPED row; the call is recorded here but not linked.
void D2Common_4D870_Forward(uint32_t nArg1, int32_t* pArg2, uint32_t nArg3, uint32_t nArg4,
                            int32_t nArg5, int32_t nArg6)
{
    (void)nArg1; (void)pArg2; (void)nArg3; (void)nArg4; (void)nArg5; (void)nArg6;
}

// 1.13c D2Common+0x53020 (#10079, function list name D2InitPlayerData): linear scan of the player-data
// pool (records of 0x13 dwords, first dword = owning unit) returning the record whose first dword
// equals nUnit, else NULL. Our engine keeps that same record on the unit (+0x14).
int32_t* D2Common_53020_PlayerData(const void* pUnit)
{
    if (pUnit == nullptr)
        return nullptr;
    return *reinterpret_cast<int32_t**>(const_cast<uint8_t*>(static_cast<const uint8_t*>(pUnit)) + 0x14);
}
