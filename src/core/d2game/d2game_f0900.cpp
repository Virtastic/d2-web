// Party invitation cancelled: both units get a relation node for each other; when c's relation to b is 4 (invited), each
// side's relation becomes 1 if that unit is a placed player in a party and 0 otherwise (c first, then b), and the
// party event of kind 6 goes out (b's id, and c's client when c is a player); the pair is refreshed both ways in every
// case. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GamePartyCancelEnv {
    void (*ensureNode)(uint32_t unit, uint32_t other, uint32_t game);
    uint32_t (*relation)(uint32_t unit, uint32_t other);          // low word is the relation
    uint32_t (*partyId)(uint32_t unit, uint32_t game);           // low word is the id, 0xFFFF none
    void (*setRelation)(uint32_t unit, uint32_t other, uint32_t v);
    uint32_t (*playerData)(uint32_t unit);
    void (*partyEvent)(uint32_t id, uint32_t client, uint32_t kind);
    void (*refreshPair)(uint32_t unit, uint32_t other);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t idOf(uint32_t u) { return u ? rd32(u + 0xC) : 0xFFFFFFFFu; }
uint32_t inParty(uint32_t u, const D2GamePartyCancelEnv* e)
{
    if (!u || !rd32(u + 0x80) || rd32(u) != 0) return 0;
    return (uint16_t)e->partyId(u, rd32(u + 0x80)) != 0xFFFF;
}
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_PartyCancel(uint32_t game, uint32_t b, uint32_t c, const D2GamePartyCancelEnv* e)
{
    e->ensureNode(c, b, game);
    e->ensureNode(b, c, game);
    if ((uint16_t)e->relation(c, b) == 4) {
        e->setRelation(c, b, inParty(c, e));
        e->setRelation(b, c, inParty(b, e));
        const uint32_t id = idOf(b);
        const uint32_t client = (c && rd32(c) == 0) ? rd32(e->playerData(c) + 0x9C) : 0;
        e->partyEvent(id, client, 6);
    }
    e->refreshPair(c, b);
    e->refreshPair(b, c);
}
