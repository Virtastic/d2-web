// 1.13c D2Game+0x597B0 -- EAX = whatever the caller left, EDX = event (its +0x14 id, the low word of its +0x18 value). The value must match the id: id 0x200 with 0x4E2E, 0x201 with 0x4E45..0x4E47, 0x202 with 0x4E55..0x4E57, 0x203 with 0x4E61..0x4E63, 0x208 with 0x4E23; anything else returns EAX with its low word replaced by the value. A match runs the helper 0x695F0 (EAX = the event's +0xC unit, ESI = id, stack = the event's +0 game) and returns its result, except id 0x201, which then walks the game's +0x10F4 record list for the record with id 0x1F and returns the result of 0x56CF0 (EAX = record), or 0 with none.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f597b0(GE& e, uint32_t a, uint32_t, uint32_t ev)
{
    const uint32_t id = e.rd32(ev + 0x14), w = e.rd32(ev + 0x18) & 0xffff;
    bool hit = false;
    switch (id) {
    case 0x200: hit = w == 0x4e2e; break;
    case 0x201: hit = w >= 0x4e45 && w <= 0x4e47; break;
    case 0x202: hit = w >= 0x4e55 && w <= 0x4e57; break;
    case 0x203: hit = w >= 0x4e61 && w <= 0x4e63; break;
    case 0x208: hit = w == 0x4e23; break;
    default: break;
    }
    if (!hit) return (a & ~0xffffu) | w;
    const uint32_t game = e.rd32(ev);
    const uint32_t v = e.h(0x695F0, e.rd32(ev + 0xc), 0, 0, 0, id, game);
    if (id != 0x201) return v;
    const uint32_t holder = e.rd32(game + 0x10f4);
    uint32_t n = holder ? e.rd32(holder) : 0;
    while (n && e.rd32(n) != 0x1f) n = e.rd32(n + 0xf4);
    if (!n) return 0;
    return e.h(0x56CF0, n, 0, 0, 0, 0);
}
}
