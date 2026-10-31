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

// 1.13c D2Common+0x20700 (#10101): the equipped-hand candidate of an inventory. Four arguments in
// retail: pInv, pOutUnit, pOutBodyLoc, pOutIsHeld. The current item is 0x20320 of the same inventory,
// taken here by its own rule (Current320 below) rather than passed in.
// The body-loc grid is 0x1E1B0 index 0 of the inventory's grid table (the table pointer is the
// inventory itself, inv+0x14 array, inv+0x18 count). Cell 4 (right hand) is taken when it passes
// 0x23EF0 (a non-type-4 unit, or a type-4 unit whose item data has neither 0x100 nor 0x4000 at +0x18)
// and is item type 0x2D; else cell 5 (left hand) when it passes the same 0x23EF0 test and is type 0x2D.
// *pOutBodyLoc is 4 or 5 and *pOutUnit is the unit; *pOutIsHeld is set only when the unit is the current
// item. On no hit *pOutUnit is 0. A null pInv returns 0 without writing the outputs.
// Retail reads the grid only after the 0x1020304 signature (a mismatched signature gives no cells).
namespace {
// 0x23EF0: a non-null unit whose item data is absent or carries neither flag (only for type 4).
bool Pass23EF0(uint8_t* c)
{
    if (!c) return false;
    if (W(c, 0) != 4) return true;
    uint8_t* idata = P(W(c, 0x14));
    if (!idata) return true;
    return !(W(idata, 0x18) & 0x100) && !(W(idata, 0x18) & 0x4000);
}
// 0x20320: the current item of the inventory: cell 5, else cell 4, when it is type 0x2D and its owner
// (+0x0C) equals the inventory's owner (inv+0x1C). Null when the signature fails or the owner is -1.
uint8_t* Current320(uint8_t* inv)
{
    if (!inv || W(inv, 0) != 0x01020304u || W(inv, 0x1C) == 0xFFFFFFFFu) return nullptr;
    uint8_t* grid = BodyLocGrid((uint32_t)(uintptr_t)inv);
    if (!grid) return nullptr;
    uint8_t* c5 = GridCell(grid, 5);
    if (c5 && ITEMS_CheckItemTypeId((D2UnitStrc*)c5, 0x2D) && W(c5, 0x0C) == W(inv, 0x1C)) return c5;
    uint8_t* c4 = GridCell(grid, 4);
    if (c4 && ITEMS_CheckItemTypeId((D2UnitStrc*)c4, 0x2D) && W(c4, 0x0C) == W(inv, 0x1C)) return c4;
    return nullptr;
}
}

uint32_t D2Common_20700_EquippedHandCandidate(uint32_t pInv, uint32_t* pOutUnit, uint32_t* pOutBodyLoc, uint32_t* pOutIsHeld)
{
    uint8_t* inv = P(pInv);
    if (!inv) return 0;
    *pOutIsHeld = 0;
    uint8_t* cur = Current320(inv);
    if (W(inv, 0) == 0x01020304u) {
        uint8_t* grid = BodyLocGrid(pInv);
        uint8_t* c4 = grid ? GridCell(grid, 4) : nullptr;
        if (Pass23EF0(c4) && ITEMS_CheckItemTypeId((D2UnitStrc*)c4, 0x2D)) {
            *pOutUnit = (uint32_t)(uintptr_t)c4;
            *pOutBodyLoc = 4;
            if (c4 != cur) return 1;
            *pOutIsHeld = 1;
            return 1;
        }
    }
    uint8_t* grid = W(inv, 0) == 0x01020304u ? BodyLocGrid(pInv) : nullptr;
    uint8_t* c5 = grid ? GridCell(grid, 5) : nullptr;
    if (Pass23EF0(c5) && ITEMS_CheckItemTypeId((D2UnitStrc*)c5, 0x2D)) {
        *pOutUnit = (uint32_t)(uintptr_t)c5;
        *pOutBodyLoc = 5;
        if (c5 != cur) return 1;
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
