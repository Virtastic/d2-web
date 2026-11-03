// The hostile delay one player holds against another, on retail's 32-bit layout (the vendored typed form is
// PLAYERLIST_GetHostileDelay in d2moo/src/D2Game/PLAYER/PlayerList.cpp). EDI = the player, ESI = the other player
// or null. Without another player the player data's +0x160 tick is the answer; otherwise the player-list chain at
// player data +0x38 (guid, -, delay, -, next) is walked for the other's GUID (+0xC), and a player asked about
// itself answers 0.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2GF00E0Env {
    uint32_t (*playerData)(uint32_t unit);   // D2Common #10920 (thunk 0xA4C0)
};

uint32_t PLAYERLIST_GetHostileDelay32(uint32_t player, uint32_t other, const D2GF00E0Env& env)
{
    if (!other) {
        const uint32_t pd = env.playerData(player);
        if (pd) return rd32(pd + 0x160);
    }
    const uint32_t pd = env.playerData(player);
    const uint32_t guid = other ? rd32(other + 0xC) : 0xFFFFFFFFu;
    if (player == other) return 0;
    for (uint32_t e = rd32(pd + 0x38); e; e = rd32(e + 0x10))
        if (rd32(e) == guid) return rd32(e + 8);
    return 0;
}
