// Player relations: send one relation word (the flags unit a keeps toward unit b) to every player of the game.
// a's player data holds a singly linked list of relation nodes {other id, flags, -, state, next}. When a and b differ
// and a has a node for b, an 11-byte packet {0x8C, a id, b id, low word of the flags} goes to every client.
// A null unit has id -1. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2GameRelationSendEnv {
    uint32_t (*playerData)(uint32_t unit);
    void (*sendToAll)(uint32_t game, const uint8_t* packet);   // every client gets the packet through the relation sender
};

namespace {
uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
uint32_t idOf(uint32_t u) { return u ? rd32(u + 0xC) : 0xFFFFFFFFu; }
}

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_RelationSend(uint32_t a, uint32_t b, uint32_t game, const D2GameRelationSendEnv* e)
{
    const uint32_t pd = e->playerData(a);
    const uint32_t bid = idOf(b);
    if (a == b) return;
    uint32_t n = rd32(pd + 0x38);
    while (n && rd32(n) != bid) n = rd32(n + 0x10);
    if (!n) return;
    uint8_t pkt[12] = {};
    const uint32_t aid = idOf(a);
    const uint16_t flags = (uint16_t)rd32(n + 4);
    pkt[0] = 0x8C;
    std::memcpy(pkt + 1, &aid, 4);
    std::memcpy(pkt + 5, &bid, 4);
    std::memcpy(pkt + 9, &flags, 2);
    e->sendToAll(game, pkt);
}
