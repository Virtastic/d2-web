// 1.13c D2Common implementation batch 6: 0x500E0, 0x32820, 0x20700, 0x5A850, 0x2A170, 0x1FD40, 0x3D160, 0x3B000.
// The functions whose retail operands are passed in registers the decompiler drops (inv.fastcall ECX/EDX,
// the 0x51470 / 0x38A80 / 0x1100 hidden operands) take those operands resolved, in the style of sub_33c20.cpp.
#include <cstddef>
#include <cstdint>
#include "D2Items.h"
#include "Units/Units.h"
#include "DataTbls/SequenceTbls.h"

namespace {
inline uint8_t* P(uint32_t a) { return (uint8_t*)(uintptr_t)a; }
inline uint32_t& W(uint8_t* b, int o) { return *(uint32_t*)(b + o); }
// Raw 32-bit-image layouts used below (retail offsets only; our wasm image is native-width).
struct Inv32 { uint8_t pad0[0x14]; uint32_t pGrids; uint32_t nGridCount; };
struct Grid32 { uint32_t pItem; uint8_t pad0[8]; uint32_t ppItems; };
struct Node32 { uint16_t nId; uint8_t pad0[2]; int32_t nKind; uint32_t pNext; };
static_assert(offsetof(Inv32, pGrids) == 0x14 && offsetof(Inv32, nGridCount) == 0x18, "0x1FD40/0x20700 inventory grids");
static_assert(offsetof(Grid32, ppItems) == 0x0C, "0x1FD40/0x20700 grid cells");
static_assert(offsetof(Node32, nKind) == 4 && offsetof(Node32, pNext) == 8, "0x3D160 list node");
// Grid 0 (INVGRID_BODYLOC); the same grid 0x20320 and 0x20700 read through 0x1E1B0 (verified layout in inv_place.cpp).
uint8_t* BodyLocGrid(uint32_t pInv)
{
    uint8_t* inv = P(pInv);
    if (!inv || W(inv, 0x18) < 1) return nullptr;
    return P(W(inv, 0x14));
}
uint8_t* GridCell(uint8_t* grid, int n)
{
    uint32_t cells = W(grid, 0x0C);
    return cells ? P(W(P(cells), n * 4)) : nullptr;
}
}

// 1.13c D2Common+0x500E0 (#10096)
// Unit predicate: a type-1 (player) unit whose animation mode (+0x10) is 0xC, that the 0x33C20 table-bit
// test misses (that call gets sgptDataTables+0x150 and takes its owner in a register) and that answers
// non-zero for ids 7 and 0xC through 0x51470. 0x51470's unit operand is register-only too, so both stat
// results and the 0x33C20 result are passed in resolved.
uint32_t D2Common_500E0_PlayerModeCWithSkillFlags(uint32_t pUnit, uint32_t nTableTest, uint32_t nFlag7, uint32_t nFlagC)
{
    uint8_t* u = P(pUnit);
    if (!u || W(u, 0) != 1 || W(u, 0x10) != 0xC) return 0;
    if (nTableTest != 0 || nFlag7 == 0 || nFlagC == 0) return 0;
    return 1;
}

// 1.13c D2Common+0x32820 (#10099)
// Restart the unit's animation sequence from its seq record: pAnimSeq = record->pAnimSeqTxtRecord; when
// that exists the record's frame counts go to +0x34 (+4 << 8) and +0x48 (+8 << 8), frame 0 / speed 0x100,
// then 0x2F100 runs ComputeSequenceAnimation(pAnimSeq, target 0, current 0), writing mode (+0x40), event
// (nActionFrame) and frame (+0x44, << 8), and the unit gets UNITFLAG_SQGFXCHANGE (0x4000 at +0xC4).
// 1.13c fetches the record again before each field; the record has no side effects, so one fetch is equal.
void D2Common_32820_InitializeSequence(D2UnitStrc* pUnit)
{
    if (!pUnit) return;
    D2AnimSeqRecordStrc* rec = DATATBLS_GetSeqRecordFromUnit(pUnit);
    pUnit->pAnimSeq = rec ? rec->pAnimSeqTxtRecord : nullptr;
    if (!pUnit->pAnimSeq) return;
    pUnit->dwSeqFrameCount = (uint32_t)rec->nSeqFramesCount << 8;
    pUnit->dwSeqFrame = 0;
    pUnit->dwSeqSpeed = 0x100;
    pUnit->dwFrameCountPrecise = (uint32_t)rec->nFramesCount << 8;
    uint32_t nMode = 0, nFrame = 0;
    int32_t nDirection = 0, nEvent = 0;
    DATATBLS_ComputeSequenceAnimation(pUnit->pAnimSeq, 0, 0, &nMode, &nFrame, &nDirection, &nEvent);
    pUnit->dwSeqMode = nMode;
    pUnit->nActionFrame = (uint8_t)nEvent;
    pUnit->nSeqCurrentFramePrecise = (int32_t)(nFrame << 8);
    pUnit->dwFlags |= UNITFLAG_SQGFXCHANGE;
}

// 1.13c D2Common+0x20700 (#10101)
// Equipped-hand candidate of an inventory: right-hand cell 4 of the body-loc grid, else left-hand cell 5.
// Cell 4 must be a type-4 unit whose item data (+0x14) is absent or has neither 0x100 nor 0x4000 set at
// item data +0x18, and must be of item type 0x2D (0x24430); cell 5 must pass 0x23EF0 (CanBeEquipped) and
// type 0x2D. On a hit *pOutUnit is the unit and *pOutBodyLoc 4/5; *pOutIsHeld is set (and stays 0
// otherwise) when the candidate is pCurrent, the item 0x20320 already reports for this inventory (own row,
// passed in). On no hit *pOutUnit is 0. Retail dereferences the grid only after the 0x1020304 signature.
// ponytail: a null cell skips the 0x23EF0 call, which retail makes; a null item only loses that call.
uint32_t D2Common_20700_EquippedHandCandidate(uint32_t pInv, uint32_t pCurrent, uint32_t* pOutUnit, uint32_t* pOutBodyLoc, uint32_t* pOutIsHeld)
{
    uint8_t* inv = P(pInv);
    if (!inv) return 0;
    *pOutIsHeld = 0;
    uint8_t* cur = P(pCurrent);
    uint8_t* cand = nullptr;
    if (W(inv, 0) == 0x01020304u) {
        uint8_t* grid = BodyLocGrid(pInv);
        cand = grid ? GridCell(grid, 4) : nullptr;
        if (cand) {
            uint8_t* idata = P(W(cand, 0x14));
            bool bItem = W(cand, 0) == 4;
            bool bFlags = !(idata && (W(idata, 0x18) & 0x100)) && !(idata && (W(idata, 0x18) & 0x4000));
            if (bItem && bFlags && ITEMS_CheckItemTypeId((D2UnitStrc*)cand, 0x2D)) {
                *pOutUnit = (uint32_t)(uintptr_t)cand;
                *pOutBodyLoc = 4;
                if (cand != cur) return 1;
                *pOutIsHeld = 1;
                return 1;
            }
        }
        grid = BodyLocGrid(pInv);
        cand = grid ? GridCell(grid, 5) : nullptr;
    }
    if (cand && ITEMS_CanBeEquipped((D2UnitStrc*)cand) && ITEMS_CheckItemTypeId((D2UnitStrc*)cand, 0x2D)) {
        *pOutUnit = (uint32_t)(uintptr_t)cand;
        *pOutBodyLoc = 5;
        if (cand != cur) return 1;
        *pOutIsHeld = 1;
        return 1;
    }
    *pOutUnit = 0;
    return 0;
}

// 1.13c D2Common+0x5A850 (#10103)
// Unit-to-string via a static short table: when the table count (a D2Common global) is positive, 0x1100
// rolls on the unit's seed (+0x20) and the roll indexes a ushort table; a non-zero entry is resolved
// through D2Lang#10003 (import thunk 0x92E8) and returned. A null unit, an empty table, a zero entry or a
// null string gives the static empty string (0x6FDF1484). The table entry and the D2Lang result are
// register-only in retail, so they are passed in resolved.
uint32_t D2Common_5A850_GetRolledString(uint32_t pUnit, uint32_t nTableCount, uint32_t nRolledTableEntry, uint32_t pResolvedString, uint32_t pEmptyString)
{
    if (!pUnit) return pEmptyString;
    if (nTableCount == 0 || nRolledTableEntry == 0 || pResolvedString == 0) return pEmptyString;
    return pResolvedString;
}

// 1.13c D2Common+0x2A170 (#10104)
// Unit gate over two ItemStatCost rows (70 = 0x46 and 125 = 0x7D, stride 0x144, table at sgptDataTables
// +0xBCC, count +0xBD4; each query is guarded by the row count and a non-null row): with the unit's stat
// list (+0x5C) present, row 70 above 0 gives 1; else row 125 non-zero writes stat 0x46 = 0 through 0x3A740
// and gives 1; else the unit's D2SetStatWithNoTest (0x260F0) result below 1 gives 1. 0x38A80's stat-list
// operand and the 0x260F0 result are register-only in retail, so they are passed in resolved.
uint32_t D2Common_2A170_StatListGate(uint32_t pUnit, int32_t nRow70Total, int32_t nRow125Total, int32_t nSetStatResult)
{
    uint8_t* u = P(pUnit);
    if (u) {
        if (W(u, 0x5C)) {
            if (nRow70Total > 0) return 1;
            if (nRow125Total != 0) return 1;
        }
        if (nSetStatResult < 1) return 1;
    }
    return 0;
}

// 1.13c D2Common+0x1FD40 (#10105)
// First equipped (body-loc grid 0, list head at grid+0x00) item of item type pItemType at or after
// pStartAfter (null: from the head), that 0x24430 accepts, and whose path field (+0x2C) value is below 0xB:
// dword at +0x0C for unit types 2, 4 and 5, otherwise the head's own value when the path is null, else the
// word at +0x02. The list continues at item data +0x70; a non-item, a null item data or the -0x5C sentinel
// ends the search. Returns the item unit or null.
uint32_t D2Common_1FD40_FindBodyGridItemOfType(uint32_t pInv, uint32_t nItemType, uint32_t pStartAfter)
{
    uint8_t* inv = P(pInv);
    if (!inv || W(inv, 0) != 0x01020304u) return 0;
    uint8_t* grid = BodyLocGrid(pInv);
    if (!grid) return 0;
    uint8_t* it = P(W(grid, 0x00));
    if (pStartAfter) {
        while (it && (uint32_t)(uintptr_t)it != pStartAfter) {
            if (W(it, 0) != 4) return 0;
            uint8_t* idata = P(W(it, 0x14));
            if (!idata || W(it, 0x14) == 0xFFFFFFA4u) return 0;
            it = P(W(idata, 0x70));
        }
    }
    while (it) {
        if (ITEMS_CheckItemTypeId((D2UnitStrc*)it, (int)nItemType)) {
            uint32_t nType = W(it, 0);
            uint32_t nValue;
            uint8_t* path = P(W(it, 0x2C));
            if (nType == 2 || (nType > 3 && nType < 6)) nValue = W(path, 0x0C);
            else if (!path) return (uint32_t)(uintptr_t)it;
            else nValue = *(uint16_t*)(path + 0x02);
            if (nValue < 0xB) return (uint32_t)(uintptr_t)it;
        }
        if (W(it, 0) != 4) return 0;
        uint8_t* idata = P(W(it, 0x14));
        if (!idata || W(it, 0x14) == 0xFFFFFFA4u) return 0;
        it = P(W(idata, 0x70));
    }
    return 0;
}

// 1.13c D2Common+0x3D160 (#10109)
// nIndex-th entry of the list at pOwner+0x08 whose dword at +0x04 equals 2: returns the node's low word
// (its id at +0x00). An empty or exhausted list returns 0xE8C.
uint32_t D2Common_3D160_NthListEntryOfKind2(uint32_t pOwner, int32_t nIndex)
{
    uint8_t* node = P(W(P(pOwner), 0x08));
    int32_t i = 0;
    for (;;) {
        if (!node) return 0xE8C;
        if (W(node, 0x04) == 2) {
            if (i == nIndex) return *(uint16_t*)node;
            ++i;
        }
        node = P(W(node, 0x08));
    }
}

// 1.13c D2Common+0x3B000 (#10118)
// Stores nValue into the dword at pObject+0x4C. A null object is fatal in retail (Fog#10265(0x67F)
// through Fog#10024, then _exit(-1)); the fatal path is not modelled, so a null object returns.
void D2Common_3B000_SetField4C(uint32_t pObject, uint32_t nValue)
{
    uint8_t* o = P(pObject);
    if (!o) return;
    W(o, 0x4C) = nValue;
}
