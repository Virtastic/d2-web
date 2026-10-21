// 1.13c D2Game+0x23100 -- weighted-table row eligibility test (callee of the 0x23980 roll).
// Host model as d2game_batch_46e80.cpp: records opaque, retail 32-bit offsets; register args
// (ECX `thisPtr`) explicit; unmapped/register-arg callees are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint8_t  rd8 (uint32_t a, uint32_t o) { return *((const uint8_t*)(uintptr_t)a + o); }
static inline uint16_t rd16(uint32_t a, uint32_t o) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline uint32_t rd32(uint32_t a, uint32_t o) { return rd32((const void*)(uintptr_t)a, o); }

struct D2GameBatch23100Env {
    const uint8_t* tables;      // sgptDataTables: MonStats rows +0xA78 (stride 0x1A8), count +0xA80
    const uint8_t* bitMasks;    // gdwBitMasks (dword-indexed)
    uint32_t (*sub2790)(void* thisPtr, int32_t index);  // 0x2790, index = entry word
    uint32_t (*sub1560)(void* thisPtr, int32_t v);      // 0x1560
    uint32_t (*sub15D0)(void* thisPtr, int32_t flag);   // 0x15D0 MONSTERUNIQUE_CheckMonStatsFlag
};

// Entry (param_2): +4 u16 weight cap, +6 enabled, +9 kind (1/2/3), +10 two s16 ids. Rejects when
// disabled, or capped (>=100) without param_3, or any positive id satisfies sub2790; kind 1 needs
// sub1560(4), kind 2 fails on MonStats row flag bit1 (bitMasks[1]) or sub15D0(0x11), kind 3 needs
// sub1560(2). Returns 1 when eligible.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2game_23100(void* thisPtr, uint32_t param1, uint32_t param2,
                                                 int32_t param3, const D2GameBatch23100Env* env)
{
    if (rd8(param2, 6) == 0 || (param3 == 0 && rd16(param2, 4) > 99)) return 0;
    for (int i = 0; i < 2; ++i) {
        const int16_t id = (int16_t)rd16(param2, 10 + i * 2);
        if (id < 1) break;
        if (env->sub2790(thisPtr, id) != 0) return 0;
    }
    const uint8_t kind = rd8(param2, 9);
    if (kind == 1) {
        if (env->sub1560(thisPtr, 4) == 0) return 0;
    } else if (kind == 2) {
        if (param1 != 0) {
            thisPtr = (void*)(uintptr_t)rd32(param1, 4);
            const int32_t id = (int32_t)(uintptr_t)thisPtr;
            if (id >= 0 && id < (int32_t)rd32(env->tables, 0xA80)) {
                const uint32_t row = rd32(env->tables, 0xA78) + (uint32_t)id * 0x1A8;
                if (row != 0 && (rd8(row, 0xC) & rd32(env->bitMasks, 4)) != 0) return 0;
            }
        }
        if (env->sub15D0(thisPtr, 0x11) != 0) return 0;
    } else if (kind == 3) {
        if (env->sub1560(thisPtr, 2) == 0) return 0;
    }
    return 1;
}
