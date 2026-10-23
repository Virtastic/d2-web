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
// table (RVA 0x94E78): {0,1},{1,0},{1,0},{-1,0}.
uint32_t D2Common_1A330_MonsterChainPair(const void* pUnit, uint32_t* pOutChain, uint32_t* pOutMax)
{
    static_assert(offsetof(D2MonStatsTxt, nChainId) == 0x4B, "monsters.txt nChainId");
    static const uint32_t kChainPair[4][2] = { { 0, 1 }, { 1, 0 }, { 1, 0 }, { 0xFFFFFFFFu, 0 } };
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

// 1.13c D2Common+0x2A210 (#10071): sum of repair/recharge costs over a unit's equipped items.
// pUnit+0x60 is the inventory (first dword 0x1020304). Slot byte offsets 0..0x30 step 4 index the
// pointer array at +0xC of the table 0x1E1B0 returns for module data 0x6FDED378. An item is
// flagged when (a) 0x27090 and 0x26D40 hold and 0x38FF0(item) is non-zero and differs from the
// stat total of record 0x49 (+0x5B20, only if the stat list +0x5C exists and count >= 0x49), or
// (b) 0x24F00 holds and the quantity-record 0x47 total (+0x5898, count >= 0x47) is below
// 0x260F0(item), or (c) 0x24FF0 succeeds with its out value 0. Flagged items add
// 0x28E80(item, nArg4, pArg5, nArg3, 3) and call pfn(pUnit). Callees are env-supplied (separate rows).
struct D2Common2A210Env {
    uint8_t* (*getTable)();                                // 0x1E1B0(&DAT_6fded378)
    int32_t  (*statTotal)(const uint8_t* pRecord);         // 0x38A80
    int32_t  (*isUsable)(uint8_t* pItem);                  // 0x27090
    int32_t  (*hasDurability)(uint8_t* pItem);             // 0x26D40
    int32_t  (*baseDurability)(uint8_t* pItem);            // 0x38FF0
    int32_t  (*isQuantityItem)(uint8_t* pItem);            // 0x24F00
    int32_t  (*maxQuantity)(uint8_t* pItem);               // 0x260F0
    int32_t  (*checkStatus)(uint8_t* pItem, int32_t* pOut);// 0x24FF0
    int32_t  (*cost)(uint8_t* pItem, int32_t a4, uint32_t* a5, int32_t a3, int32_t mode); // 0x28E80
};
extern "C" int32_t D2Common_2A210_InventoryRepairCount(const D2Common2A210Env* e, void* pUnit, int32_t nArg3,
    int32_t nArg4, uint32_t* pArg5, void (*pfn)(void*))
{
    if (pUnit == nullptr)
        return 0;
    const int32_t* pInv = *reinterpret_cast<const int32_t* const*>(static_cast<uint8_t*>(pUnit) + 0x60);
    if (pInv == nullptr)
        return 0;
    const uint8_t* t = reinterpret_cast<const uint8_t*>(sgptDataTables);
    const int32_t nCount = *reinterpret_cast<const int32_t*>(t + 0xBD4);
    const uint8_t* pBase = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(t + 0xBCC)));
    int32_t nSum = 0;
    for (int32_t nSlot = 0; nSlot < 0x34; nSlot += 4)
    {
        if (pInv[0] != 0x1020304)
            continue;
        const uint8_t* pTbl = e->getTable();
        if (!pTbl)
            continue;
        uint8_t* pItem = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(
            *reinterpret_cast<const uint32_t*>(*reinterpret_cast<const uint8_t* const*>(pTbl + 0xC) + nSlot)));
        if (!pItem)
            continue;
        const bool bStats = *reinterpret_cast<const uint32_t*>(pItem + 0x5C) != 0;
        bool bFlag = false;
        if (e->isUsable(pItem) && e->hasDurability(pItem))
        {
            const int32_t nBase = e->baseDurability(pItem);
            const int32_t nCur = (bStats && nCount >= 0x49 && pBase + 0x5B20) ? e->statTotal(pBase + 0x5B20) : 0;
            if (nBase != 0 && nCur != nBase)
                bFlag = true;
        }
        if (e->isQuantityItem(pItem))
        {
            const int32_t nMax = e->maxQuantity(pItem);
            const int32_t nCur = (bStats && nCount >= 0x47 && pBase + 0x5898) ? e->statTotal(pBase + 0x5898) : 0;
            if (nCur < nMax)
                bFlag = true;
        }
        int32_t nOut = 0;
        if ((e->checkStatus(pItem, &nOut) != 0 && nOut == 0) || bFlag)
        {
            nSum += e->cost(pItem, nArg4, pArg5, nArg3, 3);
            if (pfn)
                pfn(pUnit);
        }
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
// Needs a UNIT_ITEM (+0x00 == 4) with item data (+0x14) whose flags (+0x18) carry 0x10 and not 0x400000.
// Then 0x24FF0(unit,&out): if it answers 0, or answers 1 with out != 0, go on; otherwise true.
// Going on: the 0x1300 row for ITEMS_GetItemType(unit) must exist and have byte +8 set (else false);
// then 0x24F00(unit) ? true : ITEMS_HasDurability(unit).
// The callees are other rows (stat-list scan, row getter, 0x24F00), handed in as function pointers.
extern "C" uint32_t D2Common_27090_ItemCheck(const void* pUnit,
    int32_t (*statScan)(const void*, uint32_t*), const uint8_t* (*itemRow)(int32_t),
    uint32_t (*itemOk)(const void*), uint32_t (*hasDurability)(const void*), int32_t (*itemType)(const void*))
{
    if (pUnit == nullptr || *static_cast<const int32_t*>(pUnit) != 4)
        return 0;
    const uint8_t* pItemData = *reinterpret_cast<const uint8_t* const*>(static_cast<const uint8_t*>(pUnit) + 0x14);
    if (pItemData == nullptr)
        return 0;
    const uint32_t dwFlags = *reinterpret_cast<const uint32_t*>(pItemData + 0x18);
    if ((dwFlags & 0x10) == 0 || (dwFlags & 0x400000) != 0)
        return 0;
    uint32_t nOut = 0;
    const int32_t nScan = statScan(pUnit, &nOut);
    if (nScan == 0 || nOut != 0) {
        const uint8_t* pRow = itemRow(itemType(pUnit));
        if (pRow == nullptr || pRow[8] == 0)
            return 0;
        if (itemOk(pUnit) == 0)
            return hasDurability(pUnit) != 0;
    }
    return 1;
}

// 1.13c D2Common+0x3CD80 (#10093): if pUnit is non-null, calls 0x455B0(a2, *(int*)(pUnit+0x10), a2).
// ponytail: 0x455B0 is a still-UNMAPPED row; the call is recorded here but not linked.
void D2Common_3CD80_Forward(const void* pUnit, uint32_t nArg)
{
    (void)pUnit; (void)nArg;
}

// 1.13c D2Common+0x4D870 (#10078): thin forwarder to 0x4D140 with the argument list
// (a1, a2, NULL, a3, a4, a4, a5, a6, 1).
struct D2Common4D140Env;
extern "C" int* D2Common_4D140_FindFreeSpot(const D2Common4D140Env* e, void* pLevel, int* pPos, uint32_t* pUnit,
    uint32_t p4, uint32_t p5, uint32_t p6, int p7, int p8, int p9);
// The 0x4D140 env (callee function pointers) is passed in; the retail forwarder has them implicitly.
int* D2Common_4D870_Forward(const D2Common4D140Env* e, void* pLevel, int* pPos, uint32_t nArg3,
                            uint32_t nArg4, int32_t nArg5, int32_t nArg6)
{
    return D2Common_4D140_FindFreeSpot(e, pLevel, pPos, 0, nArg3, nArg4, nArg4, nArg5, nArg6, 1);
}

// 1.13c D2Common+0x53020 (#10079, function list name D2InitPlayerData): linear scan of the player-data
// pool (DAT 0x1DF0880 base, 0x1DF0884 count; records of 0x13 dwords, first dword = key) returning
// the record whose first dword equals nKey, else NULL.
static int32_t* s_pPlayerDataPool = nullptr;
static int32_t s_nPlayerDataCount = 0;
int32_t* D2Common_53020_PlayerData(int32_t nKey)
{
    int32_t* p = s_pPlayerDataPool;
    for (int32_t i = 0; i < s_nPlayerDataCount; ++i, p += 0x13)
        if (*p == nKey)
            return p;
    return nullptr;
}
