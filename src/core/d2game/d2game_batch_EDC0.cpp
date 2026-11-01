// src/core/d2game/d2game_batch_EDC0.cpp -- 1.13c D2Game.dll row 0x0EDC0, from the cached decompile
// (build-wasm/ghidra-d2game/decomp/0xEDC0.c) and the retail bytes at 0x6FC2EDC0 in
// build-wasm/d2game-113c.objdump. Alpha batch lane (OBJECTIVE v8.3): implementation + marker only,
// no spec, no vector.
//
// Register-only arguments: the game arrives in EAX (the +0x1B24 unique mask) and the item unit in EBX.
// They are explicit parameters below. Callees are seams on D2GameEDC0Env in retail argument order.
// Prior art: D2MOO 1.10f ITEMS_CanUniqueItemBeDropped (src/core/d2moo/src/D2Game/ITEMS/Items.cpp,
// D2Game.0x6FC4D4E0) keeps the same body: NOLIMIT unique flag, nQuest, "unique already dropped", the
// 0..4096 file-index bound and the game's 4097-bit unique mask.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

// Host seams for the callees and the module tables the cached decompile names.
struct D2GameEDC0Env {
    // Host-native view of sgptDataTables' UniqueItems array (retail fields +0xC24/+0xC28,
    // stride 0x14C). The retail 32-bit pointer is resolved by the caller; it is never reinterpreted
    // as a wasm64 host pointer here.
    const uint8_t* uniqueItems;
    uint32_t uniqueItemCount;
    // gdwBitMasks (D2Game+0xF8BE0 slot, 32 dwords, 1 << i; retail indexes it by byte offset).
    const uint8_t* bitMasks;
    void*    (*getItemsTxtRecord)(int32_t classId);        // 0xA53E D2Common #10695 DATATBLS_GetItemsTxtRecord
    int32_t  (*getFileIndex)(void* item);                  // 0xA874 D2Common #10620 ITEMS_GetFileIndex
    int32_t  (*subE800)(void* game, void* item);           // 0xE800 unique already dropped in the game mask
};

// 1.13c D2Game+0x0EDC0
// EAX = game, EBX = item. Null item: the item class id is -1. Returns 0 when there is no ItemsTxt
// record or the item's file index is out of the UniqueItems range; returns 1 without touching the
// mask when the UniqueItems row carries the NOLIMIT flag (bit 1). Otherwise a quest item (nQuest) or
// a unique id the game's mask does not already hold sets that bit -- always for a non-unique item,
// since 0xE800 only returns non-zero for a unique one -- unless the id is above 4096. Every path
// after the flag test returns 1.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_edc0(void* game, void* item, const D2GameEDC0Env* env)
{
    const void* itemsTxt = env->getItemsTxtRecord(item ? (int32_t)rd32(item, 4) : -1);
    if (!itemsTxt) return 0;
    const int32_t uniqueId = env->getFileIndex(item);
    if (uniqueId < 0 || (uint32_t)uniqueId >= env->uniqueItemCount || !env->uniqueItems) return 0;
    const uint32_t rowOffset = (uint32_t)uniqueId * 0x14Cu;
    const uint8_t* const row = env->uniqueItems + rowOffset;
    if ((rd8(row, 0x2C) & rd8(env->bitMasks, 4)) != 0) return 1;
    if (rd8(itemsTxt, 0x12A) == 0 && env->subE800(game, item) != 0) return 0;
    if (uniqueId > 0x1000) return 0;
    const uint32_t word = (uint32_t)uniqueId >> 5;
    uint8_t* const maskWord = static_cast<uint8_t*>(game) + 0x1B24u + word * 4u;
    uint32_t flags; std::memcpy(&flags, maskWord, 4);
    flags |= rd32(env->bitMasks, ((uint32_t)uniqueId & 0x1F) * 4);
    std::memcpy(maskWord, &flags, 4);
    return 1;
}
