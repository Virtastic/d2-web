// Player relations: for every node of unit's relation list in the pending state (2), look the other unit up in the
// game's unit hash (bucket id & 0x7F, chained through +0xE4); when it is there, the node's state becomes 1 if that
// unit is in a party (party id not 0xFFFF) and 0 otherwise, and the pair is refreshed. A pending node with no game
// is a fatal error. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameRelationRefreshEnv {
    uint32_t (*playerData)(uint32_t unit);
    uint32_t (*partyId)(uint32_t unit, uint32_t game);    // low word is the id, 0xFFFF none
    void (*refreshPair)(uint32_t other, uint32_t unit);
    void (*fatal)(uint32_t line);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_RelationRefreshPending(uint32_t game, uint32_t unit, const D2GameRelationRefreshEnv* e)
{
    const uint32_t pd = e->playerData(unit);
    for (uint32_t n = rd32(pd + 0x38); n; n = rd32(n + 0x10)) {
        if (rd32(n + 0xC) != 2) continue;
        const uint32_t id = rd32(n);
        if (!game) { e->fatal(0x2D3); return; }
        uint32_t u = rd32(game + 0x1120 + (id & 0x7F) * 4);
        while (u && rd32(u + 0xC) != id) u = rd32(u + 0xE4);
        if (!u) continue;
        wr32(n + 0xC, (uint16_t)e->partyId(u, game) != 0xFFFF);
        e->refreshPair(u, unit);
    }
}
