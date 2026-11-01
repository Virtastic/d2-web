// Seams for D2Game+0x37860 (count of player-slot units whose D2Common key matches).
#pragma once
#include <cstdint>
struct D2G37860Env {
    void (*iterate)(void* ctx, void* game, void (*cb)(void* ctx, void* unit));   // 0xDFC10: units whose state 7 is clear
    uint32_t (*a478)(void* unit);                                                  // D2Common #10331
    uint32_t (*a556)(uint32_t x);
    uint32_t (*field10)(void* unit);                                               // unit word at +0x10
};
uint32_t d2g_37860(void* game, uint32_t key, const D2G37860Env* env);
