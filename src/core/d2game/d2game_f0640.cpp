// Player relations: set (set != 0) or clear the mask bits of the flags unit a keeps toward unit b, then send the
// relation word {0x8C, a id, b id, low word of the new flags} to every player. Nothing happens when a and b are the
// same unit or a has no node for b. A null unit has id -1. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameRelationFlagsEnv {
    uint32_t (*playerData)(uint32_t unit);
    void (*sendToAll)(uint32_t game, const uint8_t* packet);
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
uint32_t idOf(uint32_t u) { return u ? rd32(u + 0xC) : 0xFFFFFFFFu; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_RelationSetFlags(uint32_t a, uint32_t mask, uint32_t b, uint32_t game, uint32_t set,
                                                             const D2GameRelationFlagsEnv* e)
{
    const uint32_t pd = e->playerData(a);
    const uint32_t bid = idOf(b);
    if (a == b) return;
    uint32_t n = rd32(pd + 0x38);
    while (n && rd32(n) != bid) n = rd32(n + 0x10);
    if (!n) return;
    const uint32_t f = rd32(n + 4);
    wr32(n + 4, set ? (f | mask) : (f & ~mask));
    uint8_t pkt[12] = {};
    const uint32_t aid = idOf(a);
    const uint16_t flags = (uint16_t)rd32(n + 4);
    pkt[0] = 0x8C;
    std::memcpy(pkt + 1, &aid, 4);
    std::memcpy(pkt + 5, &bid, 4);
    std::memcpy(pkt + 9, &flags, 2);
    e->sendToAll(game, pkt);
}
