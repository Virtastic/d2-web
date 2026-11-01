// D2Game: a dying player's open interaction is closed, in the retail 32-bit layout (addresses are the low 32 bits of a pointer).
#include <cstdint>
#include <cstring>

static uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static uint8_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

struct D2Game38AB0Env {
    uint32_t (*findUnit)(uint32_t game, uint32_t id, uint32_t type);         // 0xE03A0: ECX = game, EDX = id, EAX = type
    void (*stopTrade)(uint32_t game, uint32_t player);                      // 0x73AA0: EAX = game, EDI = player
    void (*resetNpc)(uint32_t player, uint32_t game);                       // 0xBCB90: ECX = player; stack game (ret 4)
    void (*objectClose)(uint32_t game, uint32_t player, uint32_t target);   // 0x6F520: EAX = game, EDX = player, ECX = target
    uint32_t (*itemsRecord)(uint32_t classId);                              // 0xA53E thunk (stdcall 1)
    void (*cubeClose)(uint32_t player, uint32_t game);                      // 0x71270: EAX = player; stack game (ret 4)
    void (*onDied)(uint32_t game, uint32_t player);                         // 0x5E200: ECX = game, EAX = player (tail call)
};

// 1.13c D2Game+0x38AB0
// EAX = player, ESI = game; plain ret. With the interact byte (+0x6C) set and the partner (type +0x68, id +0x64)
// found by 0xE03A0, the partner's type picks the close: player 0x73AA0, monster 0xBCB90, object 0x6F520, item 0x71270
// only when its items.txt code (+0x80) is "box " (the Horadric Cube). Every path ends in a tail call to 0x5E200.
void PLAYER_CloseInteractOnDeath_38AB0(uint32_t player, uint32_t game, const D2Game38AB0Env& env)
{
    const uint32_t id = rd32(player + 0x64), type = rd32(player + 0x68);
    if (rd8(player + 0x6C) != 0) {
        const uint32_t t = env.findUnit(game, id, type);
        if (t != 0) {
            switch (type) {
            case 0: env.stopTrade(game, player); break;
            case 1: env.resetNpc(player, game); break;
            case 2: env.objectClose(game, player, t); break;
            case 4: if (rd32(env.itemsRecord(rd32(t + 4)) + 0x80) == 0x20786F62u) env.cubeClose(player, game); break;
            default: break;
            }
        }
    }
    env.onDied(game, player);
}
