// src/core/d2game/d2game_batch_ca530.cpp -- three 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c): the monster-region spawn
// count adjustment on an alignment change (0xCA530), the superior-quality item roll (0x101E0) and
// the magic prefix/suffix roll (0x103C0). Base 0x6FC20000. Stage A batch lane (OBJECTIVE v9):
// implementation + marker only, no spec, no vector.
//
// Host model: a unit, an item-drop record, a monster region and a monster data block are opaque and
// addressed by their retail 32-bit byte offsets (rd8/rd16/rd32/rdptr/wr32). Register-only arguments
// (Ghidra's in_EAX/unaff_EBX/unaff_ESI/unaff_retaddr and the extra live-ins) are explicit
// parameters and are named in each function's comment. Every callee outside this batch is a seam
// pointer on D2GameBatchCA530Env in retail argument order; the D2Common ordinals are 1.13c.
// Prior art: D2MOO 1.10f MONSTER/MonsterRegion.cpp sub_6FC681C0 (0xCA530), ITEMS/Items.cpp
// sub_6FC4E430 (0x101E0) and sub_6FC4D5E0 (0x103C0); the 1.10f->1.13c deltas are noted per function.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return ((const uint8_t*)(uintptr_t)a)[o]; }
static inline int16_t  rd16(uint32_t a, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }

// Alignment values (D2MOO UNIT_ALIGNMENT_*).
static constexpr int32_t ALIGN_EVIL       = 0;
static constexpr int32_t ALIGN_NEUTRAL    = 1;
static constexpr int32_t ALIGN_GOOD       = 2;
static constexpr int32_t ALIGN_UNASSIGNED = 4;

struct D2GameBatchCA530Env {
    // 0xDFA90: SUNIT_IsDead. Retail takes the unit in EAX; the 0xCA530 decompile copies EDX there.
    // 1 when the unit is null or not in an alive mode, 0 otherwise.
    int32_t  (*sunitIsDead)(uint32_t unit);
    // 0xE210: clears all three prefix and suffix slots and the rare-affix flags (1.10f
    // sub_6FC4E1A0); ppItem in EDI.
    void     (*subE210)(uint32_t ppItem);
    // 0xEE90: ITEMS_RollItemQuality. Ghidra shows only the item argument; the second live-in
    // (unaff_EDI) dereferences +0x80 (D2ItemDropStrc::dwFlags2) and is the drop record.
    uint32_t (*subEE90)(uint32_t item, uint32_t pItemDrop);
    // 0x140B0: the low-quality item drop roll (1.10f sub_6FC54690). Ghidra shows one argument and
    // an unrecovered stack argument (iStack_4) that it dereferences as the drop record.
    uint32_t (*sub140B0)(uint32_t item, uint32_t pItemDrop);
    // 0x148A0: magic-affix roll, legacy item format (1.10f sub_6FC54240-style arg list).
    uint32_t (*sub148A0)(uint32_t item, uint32_t a1, uint32_t anyAffix, uint32_t a3, uint32_t a4);
    // 0x144D0: magic-affix roll, modern item format.
    uint32_t (*sub144D0)(uint32_t item, uint32_t a1, uint32_t anyAffix, uint32_t a3, uint32_t a4,
                         int32_t nAffix, int32_t a6);
    // 0x13DE0: assigns staffmods from the item level and the drop's dwFlags2 (1.10f sub_6FC52410).
    void     (*sub13DE0)(uint32_t pItemDrop);
    // D2Common ordinals (1.13c).
    void     (*ordinal10312)(uint32_t unit, uint32_t affix, int32_t index);          // ITEMS_AssignPrefix
    void     (*ordinal10411)(uint32_t unit);                                          // ITEMS_GetItemSeed
    void     (*ordinal10414)(uint32_t unit, uint32_t seed);                           // ITEMS_SetItemStartSeed
    uint32_t (*ordinal10513)();                                                       // SEED_GetLowSeed
    void     (*ordinal10525)(uint32_t unit, uint32_t affix, int32_t index);          // ITEMS_AssignSuffix
    void     (*ordinal10614)();                                                       // SEED_InitLowSeed
    int16_t  (*ordinal10718)(uint32_t unit);                                          // ITEMS_GetItemFormat
    void     (*ordinal10875)(uint32_t unit, uint32_t quality);                        // ITEMS_SetItemQuality
    void     (*ordinal10977)(uint32_t unit, uint32_t flag, int32_t value);            // ITEMS_SetItemFlag
};

// 1.13c D2Game+0xCA530, __fastcall(param_1 ECX, pUnit EDX, pMonRegBase stack,
//                                  unaff_EBX = old alignment, unaff_EDI = new alignment)
// pMonRegBase is the caller's pGame->pMonReg (0xF0); 0xE01A0 passes param_1 = its own first
// argument, which the body never reads. Keeping the monster-region spawn counter in step with an
// alignment change: only for a live, non-summoned monster whose region has no quest attached, and
// only when the alignment actually changed. Evil -> neutral/good decrements the count; anything ->
// evil (except from unassigned) increments it.
// 1.10f->1.13c delta: none in the rule; the 1.10f body is sub_6FC681C0.
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_fn_d2game_CA530(uint32_t param1, uint32_t pUnit, uint32_t pMonRegBase,
                        uint32_t ebxOldAlignment, uint32_t ediNewAlignment,
                        const D2GameBatchCA530Env* env)
{
    (void)param1; // ECX: unused by the body
    if (pUnit == 0 || rd32(pUnit, 0x00) != 1) return;      // UNIT_MONSTER
    if (env->sunitIsDead(pUnit) != 0) return;

    const uint32_t pMonsterData = rd32(pUnit, 0x14);
    if (pMonsterData != 0 && (rd8(pMonsterData, 0x5C) & 2) != 0) return; // wSummonerFlag & 2

    const uint32_t nLevelId = (pMonsterData == 0) ? 0 : rd32(pMonsterData, 0x58); // dwTxtLevelNo
    const uint32_t pRegion = rd32(pMonRegBase, nLevelId * 4);
    if (pRegion == 0 || rd8(pRegion, 0x2D8) != 0) return;  // nQuest

    const int32_t nOld = (int32_t)ebxOldAlignment;
    const int32_t nNew = (int32_t)ediNewAlignment;
    if (nOld == nNew) return;
    if (nOld == ALIGN_EVIL && (nNew == ALIGN_NEUTRAL || nNew == ALIGN_GOOD)) {
        wr32(pRegion, 0x2CC, rd32(pRegion, 0x2CC) - 1);    // dwMonSpawnCount
    }
    if (nNew == ALIGN_EVIL && nOld != ALIGN_UNASSIGNED) {
        wr32(pRegion, 0x2CC, rd32(pRegion, 0x2CC) + 1);
    }
}

// 1.13c D2Game+0x101E0, __fastcall(in_EAX = ppItem, unaff_EBX = pLowSeed, unaff_ESI = pItemDrop)
// The superior-quality item drop: clear the affix slots, seed the item from the caller's low seed,
// roll the quality with the drop record, force ITEMQUAL_SUPERIOR into both the item and the drop,
// write the advanced seed back, then read the item format and run the finalising roll.
// 1.10f->1.13c delta: the 1.10f tail is ITEMS_GetItemFormat + sub_6FC54690; 1.13c drops the
// GetItemFormat result and calls 0x140B0 with the drop record.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_fn_d2game_101E0(uint32_t ppItem, uint32_t pLowSeed, uint32_t pItemDrop,
                           const D2GameBatchCA530Env* env)
{
    env->subE210(ppItem);
    uint32_t item = rd32(ppItem, 0);
    env->ordinal10414(item, rd32(pLowSeed, 0));   // ITEMS_SetItemStartSeed(*ppItem, *pLowSeed)
    env->ordinal10411(item);                      // ITEMS_GetItemSeed
    env->ordinal10614();                          // SEED_InitLowSeed(seed, *pLowSeed)
    env->subEE90(item, pItemDrop);                // ITEMS_RollItemQuality
    env->ordinal10875(item, 3);                   // ITEMS_SetItemQuality(ITEMQUAL_SUPERIOR)
    wr32(pItemDrop, 0x30, 3);                     // pItemDrop->nQuality = 3
    env->ordinal10411(item);
    wr32(pLowSeed, 0, env->ordinal10513());       // *pLowSeed = SEED_GetLowSeed(seed)
    item = rd32(ppItem, 0);
    env->ordinal10718(item);                      // ITEMS_GetItemFormat
    return (int32_t)env->sub140B0(item, pItemDrop);
}

// 1.13c D2Game+0x103C0, __fastcall(ppItem ECX, pItemDrop EDX, unaff_EBX stack/live-in)
// The magic affix roll (1.10f sub_6FC4D5E0). A negative nPrefix[0] skips the prefix roll and marks
// v5; otherwise roll a prefix (0x148A0 for legacy item format 0, else 0x144D0) and assign it. A
// zero prefix marks v5; a rolled prefix with a negative nSuffix[0] skips the suffix roll. The
// suffix roll always passes v5 (EBX) as its "any affix" argument, or 1 when nSuffix[0] > 0. If both
// indices ended zero, return 0; else set IFLAG_IDENTIFIED (0x10) and run the staffmod assignment.
// Ghidra renders that last call as FUN_6fc33de0(local_4), the 0/1 flag's stack slot; its callee
// dereferences +0x0C/+0x80 as D2ItemDropStrc and every other caller passes the drop record, so the
// seam takes the drop pointer.
// 1.10f->1.13c delta: 1.10f calls ITEMS_RollMagicAffixes(...) with six visible arguments and
// sub_6FC52410(*ppItem, pItemDrop); 1.13c splits the roll by item format and passes only the drop.
extern "C" EMSCRIPTEN_KEEPALIVE
int32_t d2_fn_d2game_103C0(uint32_t ppItem, uint32_t pItemDrop, uint32_t ebxAnyAffix,
                           const D2GameBatchCA530Env* env)
{
    const int32_t nPrefix = (int32_t)rd32(pItemDrop, 0x68);  // nPrefix[0]
    uint32_t uPrefix = 0;
    uint32_t uSuffix = 0;
    int32_t  local4 = 0;                                     // v5
    bool     skipSuffix = false;

    if (nPrefix < 0) {
        local4 = 1;
    } else {
        uint32_t item = rd32(ppItem, 0);
        if (env->ordinal10718(item) == 0) {
            uPrefix = env->sub148A0(item, 1, nPrefix > 0, 1, 1);
        } else {
            uPrefix = env->sub144D0(item, 1, nPrefix > 0, 1, 1, nPrefix, 0);
        }
        env->ordinal10312(rd32(ppItem, 0), uPrefix, 0);      // ITEMS_AssignPrefix(*ppItem, p, 0)
        if ((int16_t)uPrefix == 0) {
            local4 = 1;
        } else if ((int32_t)rd32(pItemDrop, 0x74) < 0) {     // nSuffix[0]
            skipSuffix = true;
        }
    }

    if (!skipSuffix) {
        const int32_t nSuffix = (int32_t)rd32(pItemDrop, 0x74);
        if (nSuffix > 0) local4 = 1;
        uint32_t item = rd32(ppItem, 0);
        if (env->ordinal10718(item) == 0) {
            uSuffix = env->sub148A0(item, 1, ebxAnyAffix, 1, 0);
        } else {
            uSuffix = env->sub144D0(item, 1, ebxAnyAffix, 1, 0, nSuffix, 0);
        }
        env->ordinal10525(rd32(ppItem, 0), uSuffix, 0);      // ITEMS_AssignSuffix(*ppItem, s, 0)
    }

    if ((int16_t)uPrefix == 0 && (int16_t)uSuffix == 0) return 0;
    (void)local4; // only ever used by Ghidra's rendering of the 0x13DE0 call below
    env->ordinal10977(rd32(ppItem, 0), 0x10, 0);             // ITEMS_SetItemFlag(IFLAG_IDENTIFIED, 0)
    env->sub13DE0(pItemDrop);
    return 1;
}
