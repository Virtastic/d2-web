// src/core/d2game/d2game_batch_e210.cpp -- 1.13c D2Game.dll 0x0E210, transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0xE210.c). Alpha lane (OBJECTIVE v8.3):
// implementation + marker only, no vector.
//
// Host model: ppItem is an opaque host pointer whose first dword is the item unit, exactly as
// Ghidra's `*unaff_EDI` reads it. The five D2Common callees are seams on D2GameE210Env in retail
// argument order; the 1.13c import ordinals are the ones the decompile and the B0A8/B0AE/B1AA/B3B4/
// B3C6 thunks name (src/core/fn/import_thunk.cpp).
//
// Prior art (1.10f): D2MOO src/D2Game/ITEMS/Items.cpp sub_6FC4E1A0. The body is the same rule; the
// 1.13c ordinal numbering differs from 1.10f, so 10312/10525/10611/10883/10536 are the affix setters
// below rather than the 1.10f names those numbers carry (ITEMS_AssignPrefix/Suffix at 10700/10702 in
// 1.10f, ITEMS_AssignRarePrefix/Suffix at 10704/10706, ITEMS_SetFileIndex at 10733).
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }

struct D2GameE210Env {
    void (*ord10312)(void* pItem, uint16_t nPrefix, int nPrefixNo);  // ITEMS_AssignPrefix
    void (*ord10525)(void* pItem, uint16_t nSuffix, int nSuffixNo);  // ITEMS_AssignSuffix
    void (*ord10611)(void* pItem, uint16_t nPrefix);                 // ITEMS_AssignRarePrefix
    void (*ord10883)(void* pItem, uint16_t nSuffix);                 // ITEMS_AssignRareSuffix
    void (*ord10536)(void* pItem, uint32_t dwFileIndex);             // ITEMS_SetFileIndex
};

// Clear an item's affixes before its quality is rolled: all three magic prefix slots and all three
// magic suffix slots to 0, both rare-affix slots to 0, file index to -1. No return value and no
// branch on host state; the only observable is the sequence of calls.
// 1.13c D2Game+0x0E210
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_E210(void* ppItem, const D2GameE210Env* env)
{
    void* pItem = rdptr(ppItem, 0);

    for (int i = 0; i < 3; ++i) {
        env->ord10312(pItem, 0, i);
        env->ord10525(pItem, 0, i);
    }
    env->ord10611(pItem, 0);
    env->ord10883(pItem, 0);
    env->ord10536(pItem, 0xFFFFFFFFu);
}
