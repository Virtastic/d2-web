// src/core/d2game/d2game_batch_6a440.cpp -- 1.13c D2Game+0x6A440 transcribed from the cached Ghidra
// decompile (build-wasm/ghidra-d2game/decomp/0x6A440.c). Callees are seams in retail argument order.
#include <cstdint>

struct D2GameBatch6A440Env {
    int32_t  (*ordinal10666)(const int* a, const int* b);   // stdcall (param_2, param_1)
    int32_t  (*ordinal10830)(const int* unit);
    int32_t  (*ordinal10648)(const int* unit);
    uint32_t (*sub21560)(uint32_t ecx, uint32_t arg);       // FUN_6fc21560 (ECX = extraout_ECX)
    const uint8_t* (*sub21320)(void);                       // FUN_6fc21320
    int32_t  (*monStatsFromUnit)(void);                     // SUNITDMG_GetMonStatsTxtRecordFromUnit
    int32_t  (*ordinal10064)(int32_t rec, const int* unit);
    int32_t  (*ordinal10278)(const int* unit);
    uint8_t  (*sub42A80)(uint32_t arg);                     // FUN_6fc42a80
    uint32_t ecx;                                           // extraout_ECX as seen at the call
};

// 1.13c D2Game+0x6A440
bool d2game_sub_6A440(const D2GameBatch6A440Env& e, const int* p1, const int* p2)
{
    if (p1 == p2 || p1 == nullptr || p1[0] != 1) return false;
    if (e.ordinal10666(p2, p1) == 1) return false;
    if (e.ordinal10830(p1) != 0) return false;
    if (e.ordinal10648(p1) != 0) return false;
    if (p1[4] != 1 && p1[4] != 2) return false;
    if (e.sub21560(e.ecx, 2) == 0) return false;
    const uint8_t* r = e.sub21320();
    if (r == nullptr || r[0xb] == 0) return false;
    int32_t rec = e.monStatsFromUnit();
    if (rec == 0) return false;
    if (e.ordinal10064(rec, p1) != 0) return false;
    if (e.ordinal10278(p1) != 0) return false;
    return e.sub42A80(0x1f) == 0;
}
