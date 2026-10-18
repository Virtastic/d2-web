// src/core/d2game/d2game_batch_EBB0.cpp -- 1.13c D2Game.dll sub_EBB0 (0xEBB0) transcribed from the
// cached Ghidra decompile (build-wasm/ghidra-d2game/decomp/0xEBB0.c) and the retail disassembly, which
// is authoritative for the two register-only arguments: the item arrives in ESI, the drop record in
// EBX. Called from 0x10A20 (item creation) after the quality switch, for drops with ilvl > 99.
//
// This is the 1.10f ITEMS_MakeEthereal body (reference/D2MOO/source/D2Game/src/ITEMS/Items.cpp, prior
// art) carried unchanged into 1.13c: the ethereal roll applied at item creation. The drop record's
// D2ItemDropStrc::dwFlags2 at +0x80 bit 2 forbids ethereal and bit 4 forces it; dwFlags1 at +0x44
// bit 0x400000 also forces it. Only weapons (item type 0x2D) and armour (0x32) with durability,
// quality other than inferior (1) / set (5), and a zero items.txt nQuest byte are eligible.
//
// Records are opaque host objects addressed by retail 32-bit byte offsets. Every callee src/ does not
// own is a host seam on D2GameBatchEBB0Env in retail argument order; the 1.13c D2Common ordinals are
// noted at each seam (the 1.10f ordinal numbering is different).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// Host seams for the callees the cached decompile names but src/ does not implement here. The names
// are the 1.10f D2MOO crosswalk; the ordinals are the 1.13c imports.
struct D2GameBatchEBB0Env {
    // D2Common #10744 ITEMS_CheckItemTypeId(pItem, nItemType)
    int32_t  (*checkItemTypeId)(void* pItem, int32_t nItemType);
    // D2Common #10865 ITEMS_HasDurability(pItem)
    int32_t  (*hasDurability)(void* pItem);
    // D2Common #10106 ITEMS_GetItemQuality(pItem)
    int32_t  (*getItemQuality)(void* pItem);
    // D2Common #10176 items.txt nQuest accessor (0x22BC0): ITEMS_IsQuestItem
    int32_t  (*getItemQuest)(void* pItem);
    // D2Common #10411 ITEMS_GetItemSeed(pItem) -> D2SeedStrc*
    void*    (*getItemSeed)(void* pItem);
    // D2Game 0x1180 ITEMS_RollLimitedRandomNumber(pSeed, nMax): seed in ECX, nMax in EAX in retail
    uint32_t (*rollLimitedRandom)(void* pSeed, int32_t nMax);
    // D2Common #10890 ITEMMODS_ApplyEthereality(pItem) (0x46A40)
    void     (*applyEthereality)(void* pItem);
    // D2Common #10587 STATLIST_GetUnitBaseStat(pItem, nStat, nLayer)
    int32_t  (*getBaseStat)(void* pItem, int32_t nStat, int32_t nLayer);
    // D2Common #10887 STATLIST_SetUnitStat(pItem, nStat, nValue, nLayer)
    void     (*setStat)(void* pItem, int32_t nStat, int32_t nValue, int32_t nLayer);
    // D2Common #10973 STATLIST_UnitGetStatValue(pItem, nStat, nLayer)
    int32_t  (*getTotalStat)(void* pItem, int32_t nStat, int32_t nLayer);
};

static const D2GameBatchEBB0Env g_batchEBB0Env{};

// 1.13c D2Game+0xEBB0
// ESI = pItem, EBX = pItemDrop. Return when the drop forbids ethereal, the item is neither weapon nor
// armour or has no durability, its quality is inferior/set, or it is a quest item. Otherwise roll
// low32(ITEMS_GetItemSeed(pItem)) % 100 < 5 (via 0x1180), honour the two force flags, and apply
// ethereality; if the item still has durability, halve its base max durability (+1) and set current
// durability to the resulting max-durability total.
static void MakeEthereal(void* pItem, void* pItemDrop, const D2GameBatchEBB0Env* env)
{
    if ((rd8(pItemDrop, 0x80) & 0x2) != 0) return;              // dwFlags2 & ITEMDROPFLAG_NEVERETH
    if (env->checkItemTypeId(pItem, 0x2D) == 0 &&                // ITEMTYPE_WEAPON
        env->checkItemTypeId(pItem, 0x32) == 0) return;          // ITEMTYPE_ANY_ARMOR
    if (env->hasDurability(pItem) == 0) return;

    const int32_t nQuality = env->getItemQuality(pItem);
    if (nQuality == 1 || nQuality == 5) return;                  // ITEMQUAL_INFERIOR, ITEMQUAL_SET
    if (env->getItemQuest(pItem) != 0) return;

    int32_t bApply = env->rollLimitedRandom(env->getItemSeed(pItem), 100) < 5;
    if ((rd32(pItemDrop, 0x44) & 0x400000) != 0) bApply = 1;     // dwFlags1 force-ethereal
    if ((rd8(pItemDrop, 0x80) & 0x4) == 0 && bApply == 0) return; // dwFlags2 & ITEMDROPFLAG_ALWAYSETH

    env->applyEthereality(pItem);                                // D2Common #10890
    if (env->hasDurability(pItem) != 0) {
        // STAT_MAXDURABILITY = 0x49, STAT_DURABILITY = 0x48
        const int32_t nMaxDur = env->getBaseStat(pItem, 0x49, 0) / 2 + 1;
        env->setStat(pItem, 0x49, nMaxDur, 0);
        env->setStat(pItem, 0x48, env->getTotalStat(pItem, 0x49, 0), 0);
    }
}

// 1.13c D2Game+0xEBB0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_ebb0(uint32_t* r)
{
    MakeEthereal((void*)(uintptr_t)r[0], (void*)(uintptr_t)r[1], &g_batchEBB0Env);
    r[4] = 0;
}
