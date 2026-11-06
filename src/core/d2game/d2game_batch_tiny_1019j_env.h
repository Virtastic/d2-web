// Env seams of d2game_batch_tiny_1019j.cpp (0xB11A0 and its batch mates), shared with the FN probe in src/core/fn.
#pragma once
#include <cstdint>

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

// 0xB11A0 callback (fastcall ECX, EDX; stack value, arg3; the retail callee pops 8).
typedef uint32_t (*D2GameBatch1019jHitFn)(uint32_t ecx, uint32_t edx, uint32_t value, uint32_t arg3);
