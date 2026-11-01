// 1.13c D2Game+0x90450 -- free the game's proxy-record table.
// EBX = game (pool at +0x1C, table pointer at +0x1D24: count +0, array +4 of 0x44-byte records). Per record: free the chain at
// +0x18 (next at node +8, file line 0x21E), run 0x90170 on it, free the blocks at +0x2C (0x227), +0x34 (0x22C) and +0x24 (0x237)
// zeroing each pointer, clear +0x28; then free the array (0x242) and the table (0x243) and clear game+0x1D24.
#include "d2game_8d6_common.h"
struct D2Game90450Env {
    void (*freeBlock)(uint32_t pool, uint32_t ptr, uint32_t line);   // 0xA2EC
    void (*freeLists)(uint32_t game, uint32_t rec);                  // 0x90170 (EAX = game, stack = record)
};
// 1.13c D2Game+0x90450
void d2game_port_90450(uint32_t game, const D2Game90450Env* env)
{
    const uint32_t tbl = d2g8::r32(game, 0x1D24);
    const int32_t count = (int32_t)d2g8::r32(tbl, 0);
    const uint32_t arr = d2g8::r32(tbl, 4);
    for (int32_t i = 0; i < count; ++i) {
        const uint32_t rec = arr + (uint32_t)i * 0x44;
        uint32_t n = d2g8::r32(rec, 0x18);
        if (n) {
            do {
                const uint32_t next = d2g8::r32(n, 8);
                env->freeBlock(d2g8::r32(game, 0x1C), n, 0x21E);
                n = next;
            } while (n);
            d2g8::w32(rec, 0x18, 0);
        }
        env->freeLists(game, rec);
        if (d2g8::r32(rec, 0x2C)) { env->freeBlock(d2g8::r32(game, 0x1C), d2g8::r32(rec, 0x2C), 0x227); d2g8::w32(rec, 0x2C, 0); }
        if (d2g8::r32(rec, 0x34)) { env->freeBlock(d2g8::r32(game, 0x1C), d2g8::r32(rec, 0x34), 0x22C); d2g8::w32(rec, 0x34, 0); }
        if (d2g8::r32(rec, 0x24)) { env->freeBlock(d2g8::r32(game, 0x1C), d2g8::r32(rec, 0x24), 0x237); d2g8::w32(rec, 0x24, 0); }
        d2g8::w32(rec, 0x28, 0);
    }
    env->freeBlock(d2g8::r32(game, 0x1C), arr, 0x242);
    env->freeBlock(d2g8::r32(game, 0x1C), tbl, 0x243);
    d2g8::w32(game, 0x1D24, 0);
}
