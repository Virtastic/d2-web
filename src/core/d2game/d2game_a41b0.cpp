// 1.13c D2Game+0xA41B0: set a player's skill-state mode (rec+0x18) and level cap (rec+0x20),
// then send the 8-dword block {2, unit, target, 0, 0, 0x6500, 0, 0}. Written from the disassembly (the decompile drops the
// stack arguments of all three callees and the block's contents).
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2game_a41b0.h"

typedef uintptr_t A;
static inline uint32_t rd32(A p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline void wr32(A p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

// 1.13c D2Game+0xA41B0
extern "C" EMSCRIPTEN_KEEPALIVE
void d2_game_A41B0(int32_t mode, uint32_t edx, A unit, A game, A target, const D2GameA41B0Env* e)
{
    if (mode == 1) mode = 7;
    A rec = 0;
    if (unit && rd32(unit, 0) == 1 && rd32(unit, 0x14) != 0) rec = (A)rd32((A)rd32(unit, 0x14), 0x2C);
    if (mode != 0) wr32(rec, 0x18, (uint32_t)mode);
    uint8_t kind = (uint8_t)edx;
    if (kind != 0) {
        if (kind >= 0x4E) kind = 0x4D;
        ((uint8_t*)rec)[0x20] = kind;
    }
    e->o11103(unit, 0);
    uint32_t blk[8] = {};
    blk[0] = 2; blk[1] = (uint32_t)unit; blk[2] = (uint32_t)target;
    blk[5] = unit ? 0x6500u : 0x6400u;
    e->o11005((A)rd32(unit, 0x2C), 1);
    e->f95E10(game, blk, 1);
}
