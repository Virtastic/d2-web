// Relation bit update: both units get a relation node for each other; when b's flags toward a under the word mask
// differ from value, b's mask bits toward a are set (value != 0) or cleared and sent; both units are then notified
// of the other. Addresses are 32-bit, as the retail structs hold them.
#include <cstdint>
#include <emscripten.h>

struct D2GameRelationBitEnv {
    void (*ensureNode)(uint32_t unit, uint32_t other, uint32_t game);
    uint32_t (*flags)(uint32_t unit, uint32_t other, uint32_t mask);
    void (*setFlags)(uint32_t unit, uint32_t mask, uint32_t other, uint32_t game, uint32_t set);
    void (*notify)(uint32_t unit, uint32_t other);
};

extern "C" EMSCRIPTEN_KEEPALIVE void D2Game_RelationSetBit(uint32_t a, uint32_t b, uint32_t game, uint32_t mask, uint32_t value,
                                                           const D2GameRelationBitEnv* e)
{
    e->ensureNode(a, b, game);
    e->ensureNode(b, a, game);
    const uint32_t m = mask & 0xFFFF;
    if (e->flags(b, a, m) != value) e->setFlags(b, m, a, game, value);
    e->notify(a, b);
    e->notify(b, a);
}
