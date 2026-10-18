// src/core/d2game/d2game_batch_8faf0.cpp -- 1.13c D2Game.dll function transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2game/decomp/0x8FAF0.c) and checked against retail's bytes (objdump of
// retail/D2/D2SE/CORES/1.13c/D2Game.dll at 0x6FCAFAF0). Stage A batch lane (OBJECTIVE v8.3):
// implementation + marker only, no spec, no vector.
//
// Prior art (1.10f): D2MOO src/D2Game/UNIT/SUnitProxy.cpp SUNITPROXY_FreeVendorChain (0x6FCCC8C0) and
// SUNITPROXY_FreeNpcGamble (0x6FCCD190). 1.13c merges the two: one record lookup by NPC class id, then
// the gamble chain (inventory drain 0x8F470 + D2Common #10820 free) and the vendor chain, both keyed on
// the player's unit id. The 1.10f record layout is unchanged: pGamble +0x08, bGambleInit +0x0C,
// pVendorChain +0x18 (D2NpcRecordStrc); D2NpcGambleStrc {pInventory, dwGUID, pNext} and
// D2NpcVendorChainStrc {dwGUID, field_4, pNext} are both three dwords.
//
// Host model: retail records are opaque host objects addressed by their retail byte offsets (rd32/rdptr,
// wr32). Calling convention: ECX = player unit, EAX = NPC unit, game on the stack (callee pops 4); those
// registers are explicit parameters below. Every callee src/ does not own (and the vendored 0x8F5C0,
// whose D2GameStrc* signature the batch layer does not include) is a seam on D2Game8FAF0Env in retail
// argument order.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
static inline void wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// Host seams for the callees the decompile names.
struct D2Game8FAF0Env {
    // 0x8F5C0 SUNITPROXY_GetNpcRecordFromClassId(game, classId, &index), game in ECX, classId in ESI,
    // &index in EAX. Returns the record or null; class id 0 with no match is retail's fatal (0x2A3C exit).
    void*    (*getNpcRecord)(void* game, int32_t classId, int32_t* pIndex);
    // 0x8F470 FUN_6fcaf470: drain every item of an inventory, the inventory in EAX, the game on the stack.
    void     (*drainInventory)(void* inventory, void* game);
    // 0xAD96 D2Common #10820 INVENTORY_FreeInventory(inventory), the inventory in EAX.
    void     (*freeInventory)(void* inventory);
    // 0xA2EC Fog #10046 FOG_FreePool(pool, p, file, line, 0), pool in ECX, p in EDX, file/line on the
    // stack; pool is game+0x1C and the file is always "..\\Source\\D2Game\\UNIT\\sunitproxy.cpp".
    void     (*freePool)(void* pool, void* mem, uint32_t line);
};

// ECX = player unit (this), stack = game, EAX = NPC unit, void, `ret 4`.
// Look up the NPC's record by its class id (null NPC -> class id -1, which cannot match). Then:
// if record+0xC (bGambleInit), unlink the gamble node whose +4 (dwGUID) equals the player's unit id
// (player+0xC, -1 for a null player); if that node's +0 inventory is set, drain it (0x8F470) then free it
// (D2Common #10820) and zero the field; pool-free the node (sunitproxy.cpp line 0x48F) and stop.
// Then walk the vendor chain at record+0x18 for the node whose +0 equals the player's unit id, unlink it,
// pool-free it (line 0x4A7) and stop; an absent node is a plain return.
// 1.13c D2Game+0x8FAF0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_game_8faf0(void* playerUnit, void* game, void* npcUnit,
                                        const D2Game8FAF0Env* env)
{
    int32_t index = 0;
    void* record = env->getNpcRecord(game, npcUnit ? (int32_t)rd32(npcUnit, 0x04) : -1, &index); // 0x8F5C0
    if (record == nullptr) return;

    const uint32_t unitId = playerUnit ? rd32(playerUnit, 0x0C) : 0xFFFFFFFFu; // dwUnitId, -1 if null

    if (rd32(record, 0x0C) != 0) {                       // bGambleInit
        void* prev = nullptr;
        for (void* node = rdptr(record, 0x08); node != nullptr; prev = node, node = rdptr(node, 0x08)) {
            if (rd32(node, 0x04) != unitId) continue;    // dwGUID
            wr32(prev ? prev : record, 0x08, rd32(node, 0x08));
            void* inventory = rdptr(node, 0x00);
            if (inventory != nullptr) {
                env->drainInventory(inventory, game);    // 0x8F470
                env->freeInventory(inventory);           // D2Common #10820
                wr32(node, 0x00, 0);
            }
            env->freePool(rdptr(game, 0x1C), node, 0x48F); // Fog #10046
            break;
        }
    }

    void* prev = nullptr;
    for (void* node = rdptr(record, 0x18); node != nullptr; prev = node, node = rdptr(node, 0x08)) {
        if (rd32(node, 0x00) != unitId) continue;        // dwGUID
        wr32(prev ? prev : record, prev ? 0x08 : 0x18, rd32(node, 0x08));
        env->freePool(rdptr(game, 0x1C), node, 0x4A7);   // Fog #10046
        break;
    }
}
