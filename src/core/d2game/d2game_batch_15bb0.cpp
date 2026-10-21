// src/core/d2game/d2game_batch_15bb0.cpp -- 1.13c D2Game.dll sub_15BB0 transcribed from the cached
// Ghidra decompile. Implementation + marker only (batch lane). ESI = unit (register arg).
// If every node on the unit's list at (unit+0x14)->+0x30 has a zero first dword, post a command
// (Ordinal_11103, then 0x75E10 with a stack buffer) and return 1; else 0.
#include <cstdint>
#include <cstring>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void* P(uint32_t v) { return (void*)(uintptr_t)v; }

struct D2GameBatch15BB0Env {
    void (*ordinal_11103)();                      // no args in the decompile
    void (*cmd_75E10)(uint32_t p1, void* buf);    // FUN_6fc95e10; p1 uninit in retail (uStack_4), buf = stack0xffffffd8
};
D2GameBatch15BB0Env g_d2gameBatch15BB0Env;

// 1.13c D2Game+0x15BB0
extern "C" uint32_t D2Game_15BB0(void* unit) {
    D2GameBatch15BB0Env& E = g_d2gameBatch15BB0Env;
    uint32_t list = 0;  // retail derefs a null here and faults; callers never pass that
    if (unit && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) list = rd32(P(rd32(unit, 0x14)), 0x30);
    uint32_t node = rd32(P(list), 0);
    while (node) {
        if (rd32(P(node), 0) != 0) return 0;
        node = rd32(P(node), 8);
    }
    uint8_t buf[0x28] = {0};
    E.ordinal_11103();
    E.cmd_75E10(0, buf);
    return 1;
}
