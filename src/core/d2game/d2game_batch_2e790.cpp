// src/core/d2game/d2game_batch_2e790.cpp -- 1.13c D2Game.dll functions transcribed from the cached
// Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c).
//
// Records are opaque host objects addressed by retail 32-bit byte offsets (rd8/rd32/rdptr/wr8/wr32/
// wrptr). Every callee src/ does not own is a host seam on D2GameBatch13Env in retail argument order;
// a Ghidra call whose arguments arrived in registers (unaff_ESI/unaff_EDI) gets an explicit parameter
// here, noted at each seam.
#include <cstdint>
#include <cstddef>
#include <cstring>

static inline uint8_t rd8(const void* p, uint32_t o) { uint8_t v; std::memcpy(&v, (const uint8_t*)p + o, 1); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr8(void* p, uint32_t o, uint8_t v) { std::memcpy((uint8_t*)p + o, &v, 1); }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wrptr(void* p, uint32_t o, const void* v) { std::memcpy((uint8_t*)p + o, &v, sizeof v); }

// Host seams for the callees the cached decompiles name but src/ does not implement here.
struct D2GameBatch13Env {
    // 0x90170 release what a 0x44-byte game slot owns (EAX = game, stack = slot)
    void  (*releaseSlot)(void* game, void* slot);
    // 0xA448 thunk to D2Common #10387 INVENTORY_AllocInventory(pool, owner)
    void* (*allocInventory)(void* pool, void* owner);
};

// 1.13c D2Game+0x902E0
// Re-point a slot at a fresh inventory: release what the slot owns (0x90170), clear its armed-pre
// byte (+0x20) and timestamp (+0x28), allocate an unowned inventory from the game pool (+0x1C via
// D2Common #10387) and store it at slot+4; when a unit is given, attach the same inventory at
// unit+0x60. EAX = game, ESI = slot, stack = unit (may be null).
void sub_902E0_113c(void* game, void* slot, void* unit, const D2GameBatch13Env* env)
{
    env->releaseSlot(game, slot);                       // 0x90170
    wr8(slot, 0x20, 0);
    wr32(slot, 0x28, 0);
    void* inventory = env->allocInventory(rdptr(game, 0x1C), nullptr); // D2Common #10387
    wrptr(slot, 0x04, inventory);
    if (unit != nullptr) wrptr(unit, 0x60, inventory);  // CUnit+0x60 inventory pointer
}
