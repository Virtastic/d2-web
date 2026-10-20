// src/core/d2game/d2game_batch_51750.cpp -- 1.13c D2Game.dll sub_51750 transcribed from the cached
// Ghidra decompile (build-wasm/ghidra-d2game/decomp/0x51750.c). Batch lane: no spec, no vector.
//
// Flags the unit (+0xC4 |= 0x40), looks the id up in the 0x23C-byte table (count +0xBA0, base +0xB98),
// and when the +0xBC index is inside the +0xA80 table runs the apply chain. unaff_EBP is an explicit
// parameter (the disassembly fixes the live register). Callees are seams at their retail addresses.
#include <cstdint>
#include <cstring>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2GameBatch51750Env {
    const uint8_t* dataTables;                                         // *sgptDataTables
    void  (*ordinal10786)(void* p2, uint32_t v, int32_t p3, uint32_t p4);  // Ordinal_10786
    void* (*sub_A3610)(void* ebp);                                     // 0xA3610
    void  (*sub_50070)(void* ebp, int32_t p3);                         // 0x50070
    void  (*ordinal10920)(void* p2);                                   // Ordinal_10920
    void  (*sub_8E2A0)(void);                                          // 0x8E2A0
    void  (*sub_EFE40)(void* ebp);                                     // 0xEFE40
};

// 1.13c D2Game+0x51750
uint32_t D2Game_sub_51750(const D2GameBatch51750Env* env, uint32_t p1, void* p2, int32_t p3,
                          uint32_t p4, void* unaffEBP)
{
    (void)p1;
    if (p2) wr32(p2, 0xC4, rd32(p2, 0xC4) | 0x40);
    const uint8_t* row = nullptr;
    if (p3 >= 0 && (int32_t)rd32(env->dataTables, 0xBA0) > p3)
        row = (const uint8_t*)(uintptr_t)rd32(env->dataTables, 0xB98) + (uint32_t)p3 * 0x23C;
    if (p3 == 0) return 0;
    int16_t idx = rd16s(row, 0xBC);
    if (idx > -1 && (int32_t)idx < (int32_t)rd32(env->dataTables, 0xA80)) {
        env->ordinal10786(p2, rd32(row, 0xC0), p3, p4);
        if (env->sub_A3610(unaffEBP)) {
            env->sub_50070(unaffEBP, p3);
            if (p2 && rd32(p2, 0) == 0) env->ordinal10920(p2);
            env->sub_8E2A0();
            env->sub_EFE40(unaffEBP);
            return 1;
        }
    }
    return 0;
}
