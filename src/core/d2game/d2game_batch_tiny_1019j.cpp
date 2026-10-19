// src/core/d2game/d2game_batch_tiny_1019j.cpp -- four tiny 1.13c D2Game.dll functions transcribed
// from the cached Ghidra decompiles (build-wasm/ghidra-d2game/decomp/0x<RVA>.c) and the retail
// disassembly, in RVA order: 0x6FFA0 0x8AAC0 0x9E2E0 0xB11A0. The fifth row of the batch, 0xB87B0,
// was already implemented as an FN capture in src/core/fn/d2game_tiny_ret_const.cpp; only its
// marker was unparseable there and has been given a definition the function list can see.
//
// Stage A batch lane (OBJECTIVE v9): implementation + marker, no spec, no vector.
//
// Host model: retail module state is opaque and read by retail byte offsets. Register-only arguments
// (in_EAX / in_AL / unaff_EBX / unaff_retaddr) are explicit host parameters, named at each function.
// Unmapped callees are seams on D2GameBatch1019jEnv in retail argument order; the D2Common ordinals
// are 1.13c import numbers (the thunks at D2Game+0xA478 etc.), not the 1.10f names for those numbers.
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <unistd.h>
#include <emscripten.h>

static inline uint32_t rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
static inline void     wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }

[[noreturn]] static void d2game_fatal(uint32_t) { _exit(-1); }

// Host seams for the callees the cached decompiles name but src/ does not implement. Argument lists
// follow the retail call sites (ECX/EDX/EAX at the call, then the pushed stack arguments).
struct D2GameBatch1019jEnv {
    // 0x8A3E0: append size bytes of data to the context's packet buffer; the context arrives in EAX.
    void     (*fn8A3E0)(uint32_t ctx, const void* data, uint32_t size);
    // 0xE03A0: path node at [pos] of the path base; the path index arrives in EAX.
    uint32_t (*fnE03A0)(uint32_t pathBase, uint32_t pos, uint32_t pathIndex);

    // D2Common ordinals (1.13c import numbers).
    uint32_t (*ordinal10920)(uint32_t ctx);
    uint32_t (*ordinal11157)(uint32_t unit);
    uint32_t (*ordinal11106)(uint32_t unit, uint32_t arg);
    uint32_t (*ordinal10331)(uint32_t room);
    void     (*ordinal10383)(uint32_t room, void** array, uint32_t* count);
};

// 0x6FFA0: ask D2Common #10920 for the player-data record of the EAX argument, then walk the
// inventory list at +0x5C (nodes linked through +8) to its end. The walk has no side effects, so the
// only observable work is the ordinal call. EAX at entry is explicit.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_6FFA0(uint32_t ctx, const D2GameBatch1019jEnv* env) {   // 1.13c D2Game+0x6FFA0
    const uint32_t playerData = env->ordinal10920(ctx);
    if (playerData == 0) return;
    const uint32_t inventory = rd32(playerData, 0x5C);
    if (inventory == 0) return;
    for (uint32_t node = rd32(inventory, 8); node != 0; node = rd32(node, 8)) {
    }
}

// 0x8AAC0: append the two-byte record { 0x61, AL } to the packet buffer of the stack argument.
// AL at entry is explicit; ECX at the 0x8A3E0 call points at the record.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_8AAC0(uint32_t ctx, uint8_t al, const D2GameBatch1019jEnv* env) {   // 1.13c D2Game+0x8AAC0
    const uint8_t record[2] = { 0x61, al };
    env->fn8A3E0(ctx, record, 2);
}

// 0x9E2E0: __fastcall (ECX = unit, EDX = arg). When D2Common #11157 accepts the unit, answer
// #11106(unit, arg); otherwise answer 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_9E2E0(uint32_t unit, uint32_t arg, const D2GameBatch1019jEnv* env) {   // 1.13c D2Game+0x9E2E0
    if (env->ordinal11157(unit) == 0) return 0;
    return env->ordinal11106(unit, arg);
}

// The callback 0xB11A0 fires: __fastcall with ECX/EDX from the caller's first two arguments and the
// candidate value and third argument pushed left to right.
typedef uint32_t (*D2GameBatch1019jHitFn)(uint32_t ecx, uint32_t edx, uint32_t value, uint32_t arg3);

// 0xB11A0: room sweep. D2Common #10331(param_2) is the room; #10383 yields its unit array + count.
// Each unit's path node at +0x174 is resolved: live flag bit 0 at +0x3D4 uses it as is, otherwise
// 0xE03A0(+0x1A8 base, +0x170 pos, +0x16C index) re-resolves it and a zero answer clears the field
// (a null unit in the array is retail's fatal 0xFB). Every non-null node is offered to the callback;
// the first non-zero answer is returned.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2game_B11A0(uint32_t arg1, uint32_t arg2, uint32_t arg3, D2GameBatch1019jHitFn hit, const D2GameBatch1019jEnv* env) {   // 1.13c D2Game+0xB11A0
    const uint32_t room = env->ordinal10331(arg2);
    if (room == 0) return 0;
    void* array = nullptr;
    uint32_t count = 0;
    env->ordinal10383(room, &array, &count);
    for (uint32_t i = 0; i < count; ++i) {
        const uint32_t record = rd32((uint32_t)(uintptr_t)array, i * 4);
        const uint32_t innerCount = rd32(record, 0x78);
        const uint32_t innerArray = rd32(record, 0x48);
        for (uint32_t j = 0; j < innerCount; ++j) {
            const uint32_t unit = rd32(innerArray, j * 4);
            if (unit == 0) d2game_fatal(0xFB);   // Ordinal_10265(0xFB) -> Ordinal_10024 -> _exit(-1)
            uint32_t node;
            if ((rd32(unit, 0x3D4) & 1) == 0) {
                if (rd32(unit, 0x174) == 0) continue;
                node = env->fnE03A0(rd32(unit, 0x1A8), rd32(unit, 0x170), rd32(unit, 0x16C));
                if (node == 0) wr32(unit, 0x174, 0);
            } else {
                node = rd32(unit, 0x174);
            }
            if (node != 0) {
                const uint32_t answer = hit(arg1, arg2, node, arg3);
                if (answer != 0) return (int32_t)answer;
            }
        }
    }
    return 0;
}
