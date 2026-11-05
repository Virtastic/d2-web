// Party state of a pair after a leave, between two distinct units (ids compared, a null unit being -1): when a is a placed player in a
// party, both sides become 1; otherwise when a's relation to b is 4 (invited) only b's toward a becomes 2; otherwise
// a's becomes 0 and b's 1. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameRelationResetEnv {
    uint32_t (*partyId)(uint32_t unit, uint32_t game);           // low word is the id, 0xFFFF none
    uint32_t (*relation)(uint32_t unit, uint32_t other);          // low word is the relation
    void (*setRelation)(uint32_t unit, uint32_t other, uint32_t v);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t idOf(uint32_t u) { return u ? rd32(u + 0xC) : 0xFFFFFFFFu; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_RelationReset(uint32_t a, uint32_t b, const D2GameRelationResetEnv* e)
{
    if (idOf(a) == idOf(b)) return;
    const bool party = a && rd32(a + 0x80) && rd32(a) == 0 && (uint16_t)e->partyId(a, rd32(a + 0x80)) != 0xFFFF;
    if (!party && (uint16_t)e->relation(a, b) == 4) { e->setRelation(b, a, 2); return; }
    e->setRelation(a, b, party ? 1 : 0);
    e->setRelation(b, a, 1);
}
