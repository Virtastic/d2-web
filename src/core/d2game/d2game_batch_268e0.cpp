// src/core/d2game/d2game_batch_268e0.cpp -- six 1.13c D2Game.dll functions transcribed from the
// cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c). Base 0x6FC20000.
//
// Retail module state is modelled as opaque host records read by their retail byte offsets. Register
// only arguments (Ghidra's in_EAX / unaff_EDI / unaff_EBX) and stack locals the decompile could not
// tie to a parameter (0x750F0's iStack_4 / stack0xffffffd8) are explicit parameters and are named at
// each function. Unmapped callees are seams on D2GameBatch268E0Env in retail argument order; a seam
// whose decompile shows no arguments is taken to have none. The two retail data tables the decompiles
// name (the per-skill callback table at 0x6FD2F448 and the 0x6FD1BC50 block) arrive as host pointers.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (const void* p, uint32_t o) { return ((const uint8_t*)p)[o]; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }
static inline void*    rdptr(const void* p, uint32_t o) { return (void*)(uintptr_t)rd32(p, o); }  // retail pointers are 32-bit words
// Retail addresses arrive as 32-bit integers in the decompile (unaff_EDI, in_EAX, ...).
static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return rd8((const void*)(uintptr_t)a, o); }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

// Host seams for the callees and data tables the cached decompiles name but src/ does not implement.
struct D2GameBatch268E0Env {
    // 0x10F448 (DAT_6FD2F448): six callback dwords per skill/stat id; the column is param_4.
    const uint8_t* statCallbacks;
    // 0xDA750: prologue of the 0x74E60 path.
    void     (*subDA750)();
    // 0xA4A2, D2Common #11090(unit, 1).
    void     (*ordinal11090)(void* unit, uint32_t value);
    // 0x8E490: the level/threshold compared against param_1+0xA8.
    int32_t  (*sub8E490)();
    // 0xA68E, D2Common #10494(unit, 0x15).
    int32_t  (*ordinal10494)(void* unit, uint32_t value);
    // 0x8EA80(param_1, unit, 2, value, 0, 0, 0): the seven-argument send/action tail.
    void     (*subAEA80)(void* param1, void* unit, uint32_t mode, int32_t value,
                         uint32_t zero1, uint32_t zero2, uint32_t zero3);
    // 0xFBC50 (DAT_6FD1BC50): 0xC-byte entries; word 0 is a flag, word 1 the gate, word 2 the selector.
    const uint8_t* tableFBC50;
    // gdwBitMasks: the mask dwords indexed by (in_EAX & 7).
    const uint8_t* bitMasks;
    // 0x1320: resolves the record whose +0x104.. byte block is tested.
    int32_t  (*sub1320)();
    // 0xA496, Fog #11103.
    void     (*ordinal11103)(void* unit);
    // 0xDFA90: the probe behind the 0x750F0 guard.
    int32_t  (*subDFA90)(void* unit);
    // 0xA2EC, Fog #10046(file, line, 0): the per-node call of the 0x37500 list walk.
    void     (*ordinal10046)(const char* file, uint32_t line, uint32_t zero);
};

// The unit from param_2+0x04 is reset (0xDA750, then ordinal 11090(unit, 1)); when 0x8E490's value is
// at most param_1+0xA8, ordinal 10494(unit, 0x15) either sends the param_1+0xA8 + 0x2D tail, or -- for
// a type-1 unit with a +0x14 record whose first dword is non-null -- sends the +0xA8 plus the record's
// +0x4F byte (0xF when zero), indexed by param_1+0x6D unless param_1+0x6A is clear and +0x74 is zero.
// D2Game+0x74E60 (marker lives on sub_6FC64310)
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_74E60(uint32_t param1, void* param2,
                                                 const D2GameBatch268E0Env* env)
{
    void* unit = rdptr(param2, 4);                 // piVar2 = *(int **)(param_2 + 4)

    env->subDA750();
    env->ordinal11090(unit, 1);
    int32_t value = env->sub8E490();
    if (value <= (int32_t)rd32(param1, 0xA8)) {
        if (env->ordinal10494(unit, 0x15) != 0) {
            env->subAEA80((void*)(uintptr_t)param1, unit, 2, (int32_t)rd32(param1, 0xA8) + 0x2D, 0, 0, 0);
            return 1;
        }

        const uint32_t sub = unit != nullptr ? rd32(unit, 0x14) : 0;
        if (unit != nullptr && rd32(unit, 0) == 1 && sub != 0) {
            const int32_t data = (int32_t)rd32(sub, 0);
            if (data != 0) {
                uint8_t b;
                if (rd8(param1, 0x6A) == 0 && rd32(param1, 0x74) == 0) {
                    b = rd8((void*)(uintptr_t)data, 0x4F);
                }
                else {
                    b = rd8((void*)(uintptr_t)(data + 0x4F + rd8(param1, 0x6D)), 0);
                }
                if (b == 0) b = 0x0F;
                env->subAEA80((void*)(uintptr_t)param1, unit, 2, (int32_t)rd32(param1, 0xA8) + b, 0, 0, 0);
            }
        }
    }
    return 1;
}

// 0x74F20, 0x750F0 and 0x75260 run the game bodies (D2Game/MONSTER/MonsterMode*.cpp, MonsterAiStateForMode.cpp).

// 0x37500 runs the game body (D2Game/PLAYER/Player.cpp PLAYER_FreeNodeChain).
