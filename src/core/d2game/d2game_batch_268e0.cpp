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
static inline void*    rdptr(const void* p, uint32_t o) { void* v; std::memcpy(&v, (const uint8_t*)p + o, sizeof v); return v; }
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
    void     (*ordinal11103)();
    // 0xDFA90: the probe behind the 0x750F0 guard.
    int32_t  (*subDFA90)();
    // 0xA2EC, Fog #10046(file, line, 0): the per-node call of the 0x37500 list walk.
    void     (*ordinal10046)(const char* file, uint32_t line, uint32_t zero);
};

// Per-skill callback dispatch on a unit's skill list. The unit's +0x14 record holds a flag byte at
// +0x16 (bit 3) and nine skill ids at +0x1C; the walk runs only when the unit type is 1 and the first
// skill id is non-zero, and each id calls column param_4 of its six-dword table row (the param_4 == 5
// branch is the same table with the base folded by 0x14). param_1 and param_3 are unused.
// 1.13c D2Game+0x268E0
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_268E0(uint32_t param1, void* param2, uint32_t param3,
                                             int32_t param4, const D2GameBatch268E0Env* env)
{
    (void)param1;
    (void)param3;

    uint32_t sub = param2 != nullptr ? rd32(param2, 0x14) : 0;

    uint8_t flag = 0;
    if (param2 != nullptr && rd32(param2, 0) == 1 && sub != 0) {
        flag = (uint8_t)((rd8((void*)(uintptr_t)sub, 0x16) >> 3) & 1);
    }

    if (param2 == nullptr || rd32(param2, 0) != 1 || sub == 0) return;
    const uint8_t* ids = (const uint8_t*)(uintptr_t)(sub + 0x1C);
    if (ids[0] == 0) return;                        // pcVar3[0] != '\0' gates the whole walk

    for (int32_t i = 0; i < 9; ++i) {
        const uint32_t id = ids[i];
        const int32_t slot = (int32_t)id * 6 + param4;
        const uint32_t cb = rd32(env->statCallbacks, (uint32_t)(slot * 4));
        if (cb != 0) {
            ((void (*)(uint32_t, uint8_t))(uintptr_t)cb)(id, flag);
        }
    }
}

// The unit from param_2+0x04 is reset (0xDA750, then ordinal 11090(unit, 1)); when 0x8E490's value is
// at most param_1+0xA8, ordinal 10494(unit, 0x15) either sends the param_1+0xA8 + 0x2D tail, or -- for
// a type-1 unit with a +0x14 record whose first dword is non-null -- sends the +0xA8 plus the record's
// +0x4F byte (0xF when zero), indexed by param_1+0x6D unless param_1+0x6A is clear and +0x74 is zero.
// 1.13c D2Game+0x74E60
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

// Class/state probe: a non-null param_1 and a signed in_EAX below 0x10 index the 0xC-byte 0x6FD1BC50
// entries. Entry word 0 non-zero answers 1; otherwise the (word 1 == 0 && word 2 != 0) entries ask
// 0x1320 for a record and answer the mask bit (in_EAX & 7) of byte record+0x104+(in_EAX >> 3).
// 1.13c D2Game+0x74F20
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2GAME_ClassStateProbe(uint32_t param1, uint32_t eaxValue,
                                                 const D2GameBatch268E0Env* env)
{
    if (param1 != 0 && (int32_t)eaxValue < 0x10) {
        const uint32_t entry = (uint32_t)eaxValue * 0xC;
        if (rd32(env->tableFBC50, entry) != 0) return 1;
        if (rd32(env->tableFBC50, entry + 4) == 0 && rd32(env->tableFBC50, entry + 8) != 0) {
            const int32_t rec = env->sub1320();
            if (rec != 0) {
                const uint32_t mask = rd32(env->bitMasks, (eaxValue & 7) * 4);
                const uint8_t bits = rd8((void*)(uintptr_t)(rec + 0x104 + (int32_t)(eaxValue >> 3)), 0);
                return mask & (uint32_t)bits;
            }
        }
    }
    return 0;
}

// The 0x74E60 entry with a guard: ordinal 11103 runs first, and an EDI other than 0 or 0xC takes the
// 0xDFA90 probe, whose non-zero answer returns 1 without reaching 0x74E60. Otherwise param_1 (the
// decompile's iStack_4) and the local context passed as &stack0xffffffd8 reach 0x74E60, and 0 is
// returned. EDI and the two stack values are explicit here.
// 1.13c D2Game+0x750F0
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2GAME_750F0_GuardedEntry(uint32_t param1, int32_t edi, void* ctx,
                                                 const D2GameBatch268E0Env* env)
{
    env->ordinal11103();
    if (edi != 0 && edi != 0xC) {
        if (env->subDFA90() != 0) return 1;
    }
    d2_fn_d2game_74E60(param1, ctx, env);          // FUN_6fc94e60(iStack_4, &stack0xffffffd8)
    return 0;
}

// Mode rewrite on a type-1 unit: with a +0x14 record, the word at record+0x54 below 0x10 is left alone
// except that (0xD, EAX==3) pins EAX to 0xD; at or above 0x10 EAX becomes the word minus 0x10. The
// resulting value is stored back to record+0x54 (and EAX is left untouched when the record is null).
// The value arrives in EAX.
// 1.13c D2Game+0x75260
extern "C"
EMSCRIPTEN_KEEPALIVE void UNIT_SetModeFromRecord_75260(void* param1, int32_t eaxValue,
                                             const D2GameBatch268E0Env* env)
{
    (void)env;
    if (param1 == nullptr || rd32(param1, 0) != 1) return;

    const uint32_t sub = rd32(param1, 0x14);
    int32_t value = eaxValue;
    if (sub != 0) {
        const int32_t mode = (int32_t)rd32(sub, 0x54);
        if (mode < 0x10) {
            if (mode == 0xD && value == 3) value = 0xD;
        }
        else {
            value = mode - 0x10;
        }
    }
    if (sub != 0) wr32((void*)(uintptr_t)sub, 0x54, (uint32_t)value);
}

// Player-list teardown: from EBX+0x98 walk each node's +0x10 link, paying one Fog #10046
// (Player.cpp, line 0x2B, 0) in the loop, and clear the head at the end.
// 1.13c D2Game+0x37500
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_37500(void* ebx, const D2GameBatch268E0Env* env)
{
    static const char kPlayerFile[] = "..\\Source\\D2Game\\PLAYER\\Player.cpp";

    uint32_t node = rd32(ebx, 0x98);
    while (node != 0) {
        node = rd32((void*)(uintptr_t)node, 0x10);
        env->ordinal10046(kPlayerFile, 0x2B, 0);
    }
    wr32(ebx, 0x98, 0);
}
