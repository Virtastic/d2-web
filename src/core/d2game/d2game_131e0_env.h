// Env and helpers shared by the 1.13c D2Game item-affix helpers (d2game_batch_131e0.cpp and the per-function
// files split out of it: d2game_131e0.cpp, d2game_132a0.cpp, d2game_13320.cpp, d2game_135e0.cpp).
#pragma once
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint16_t rd16(const void* p, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr8 (void* p, uint32_t o, uint8_t v) { ((uint8_t*)p)[o] = v; }
static inline void     wr16(void* p, uint32_t o, uint16_t v) { std::memcpy((uint8_t*)p + o, &v, 2); }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

[[noreturn]] static void d2game_fatal(uint32_t code) { (void)code; _exit(-1); }

struct D2GameBatch131E0Env {
    // D2Common ordinals in the shape the decompile calls them.
    uint16_t (*ordinal10257)(void* item, int32_t index);   // ITEMS_GetPrefixId
    uint16_t (*ordinal11028)(void* item, int32_t index);   // ITEMS_GetSuffixId
    void*    (*ordinal10248)(int32_t index);               // DATATBLS_GetMagicAffixTxtRecord
    void*    (*ordinal10480)(void);                        // 1.13c: address constant of a D2Common global {count, rows}
    void*    (*ordinal10695)(int32_t classId);             // DATATBLS_GetItemsTxtRecord
    uint64_t* (*ordinal10411)(void* item);                 // ITEMS_GetItemSeed (8-byte LCG state)
    uint16_t (*ordinal10718)(void* item);                  // ITEMS_GetItemFormat
    int32_t  (*ordinal10106)(void* item);                  // ITEMS_GetItemQuality
    int32_t  (*ordinal10744)(void* item, int32_t typeId);  // ITEMS_CheckItemTypeId
    void*    (*ordinal10930)(void* unit, int32_t state, uint32_t flag); // STATLIST_GetStatListFromUnitStateAndFlag
    void*    (*ordinal11013)(void* pool, uint32_t flags, uint32_t timeout,
                             uint32_t ownerType, int32_t ownerId);      // STATLIST_AllocStatList
    void     (*ordinal10807)(void* unit, void* list, int32_t set);      // D2COMMON_10475_PostStatToStatList
    void     (*ordinal10188)(void* list, int32_t stat, int32_t value,
                             int32_t layer);                            // STATLIST_SetStatIfListIsValid
    const uint8_t* dataTables;                             // sgptDataTables: skills.txt at +0xB98/+0xBA0
};

// Item-seed LCG from the disassembly: product = low * 0x6AC690C5 + high, stored as one 64-bit value;
// the callers divide the low word (the other half is only carried).
static uint32_t d2game_lcg_step(uint64_t* seed)
{
    const uint64_t v = (uint64_t)(uint32_t)*seed * 0x6AC690C5u + (uint64_t)(uint32_t)(*seed >> 32);
    *seed = v;
    return (uint32_t)v;
}
