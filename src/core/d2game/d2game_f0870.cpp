// Party invitation received: both units get a relation node for each other; when c's relation to b is 0 (none), c's becomes
// 4 and b's becomes 2 and the party event of kind 5 goes out (b's id, and c's client when c is a player); the pair is
// refreshed both ways in every case. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GamePartyAcceptEnv {
    void (*ensureNode)(uint32_t unit, uint32_t other, uint32_t game);
    uint32_t (*relation)(uint32_t unit, uint32_t other);          // low word is the relation
    void (*setRelation)(uint32_t unit, uint32_t other, uint32_t v);
    uint32_t (*playerData)(uint32_t unit);
    void (*partyEvent)(uint32_t id, uint32_t client, uint32_t kind);
    void (*refreshPair)(uint32_t unit, uint32_t other);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t idOf(uint32_t u) { return u ? rd32(u + 0xC) : 0xFFFFFFFFu; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_PartyAccept(uint32_t game, uint32_t b, uint32_t c, const D2GamePartyAcceptEnv* e)
{
    e->ensureNode(c, b, game);
    e->ensureNode(b, c, game);
    if ((uint16_t)e->relation(c, b) == 0) {
        e->setRelation(c, b, 4);
        e->setRelation(b, c, 2);
        const uint32_t id = idOf(b);
        const uint32_t client = (c && rd32(c) == 0) ? rd32(e->playerData(c) + 0x9C) : 0;
        e->partyEvent(id, client, 5);
    }
    e->refreshPair(c, b);
    e->refreshPair(b, c);
}
