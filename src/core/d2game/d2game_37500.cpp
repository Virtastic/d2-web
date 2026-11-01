// D2Game player-data teardown helpers in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
// 0x37500 frees the +0x98 node chain, 0x376B0 tears the player data down, 0x376F0 builds the three intro slots.
#include "d2game_37500.h"
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static const char kPlayerFile[] = "..\\Source\\D2Game\\PLAYER\\Player.cpp";

// 1.13c D2Game+0x37500
// EBX = player data, EDI = game. Each node of the +0x98 chain (next at +0x10) goes back to the game's
// memory pool (game+0x1C) through Fog #10046 (Player.cpp, line 0x2B); the head is cleared afterwards.
void PLAYER_FreeNodeChain_37500(uint32_t playerData, uint32_t game, const D2Game37500Env& env)
{
    uint32_t node = rd32(playerData + 0x98);
    while (node != 0) {
        const uint32_t next = rd32(node + 0x10);
        env.fogFree(rd32(game + 0x1C), node, kPlayerFile, 0x2B, 0);
        node = next;
    }
    wr32(playerData + 0x98, 0);
}

// 1.13c D2Game+0x376B0
// EAX = player, ECX = game. No player data: nothing. Otherwise the +0x98 chain is freed (0x37500) and each
// of the three intro slots at +0x60 goes to 0x692D0 (ESI = slot value, EDI = game) and is cleared.
void PLAYER_DestroyIntros_376B0(uint32_t player, uint32_t game, const D2Game37500Env& env)
{
    const uint32_t pd = env.getPlayerData(player);
    if (pd == 0) return;
    PLAYER_FreeNodeChain_37500(pd, game, env);
    for (uint32_t i = 0; i < 3; ++i) {
        env.destroyIntro(rd32(pd + 0x60 + 4 * i), game);
        wr32(pd + 0x60 + 4 * i, 0);
    }
}

// 1.13c D2Game+0x376F0
// EAX = player, ECX = game. With player data, each of the three slots at +0x60 takes the result of 0x69360
// (ESI = game), and the dword at +0x6C is cleared after the loop.
void PLAYER_CreateIntros_376F0(uint32_t player, uint32_t game, const D2Game37500Env& env)
{
    const uint32_t pd = env.getPlayerData(player);
    if (pd == 0) return;
    for (uint32_t i = 0; i < 3; ++i) wr32(pd + 0x60 + 4 * i, env.createIntro(game));
    wr32(pd + 0x6C, 0);
}
