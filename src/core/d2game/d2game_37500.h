// Seams of the 0x37500 / 0x376B0 / 0x376F0 ports (src/core/d2game/d2game_37500.cpp), retail argument order.
#pragma once
#include <cstdint>
struct D2Game37500Env {
    uint32_t (*getPlayerData)(uint32_t unit);                                        // D2Common #10920 (0xA4C0)
    void (*fogFree)(uint32_t pool, uint32_t p, const char* file, int line, int zero); // Fog #10046 (0xA2EC)
    void (*destroyIntro)(uint32_t intro, uint32_t game);                             // 0x692D0: ESI, EDI
    uint32_t (*createIntro)(uint32_t game);                                          // 0x69360: ESI
};
void PLAYER_FreeNodeChain_37500(uint32_t playerData, uint32_t game, const D2Game37500Env& env);
void PLAYER_DestroyIntros_376B0(uint32_t player, uint32_t game, const D2Game37500Env& env);
void PLAYER_CreateIntros_376F0(uint32_t player, uint32_t game, const D2Game37500Env& env);
