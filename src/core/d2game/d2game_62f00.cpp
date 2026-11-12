// 1.13c D2Game+0x62F00 -- EAX = whatever the caller left, EDX = event (its +0x14 id, the low word of its +0x18 value). The value must match the id: id 0x93 with 0x2D or 0x2E, 0x94 with 0xB or 0xC, 0x96 with 0x18 or 0x19, 0x9A with 0x24 or 0x25; anything else returns EAX with its low word replaced by the value. A match returns the result of the helper 0x695F0 (EAX = the event's +0xC unit, ESI = id, stack = the event's +0 game).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f62f00(GE& e, uint32_t a, uint32_t, uint32_t ev)
{
    const uint32_t id = e.rd32(ev + 0x14), w = e.rd32(ev + 0x18) & 0xffff;
    bool hit = false;
    switch (id) {
    case 0x93: hit = w == 0x2d || w == 0x2e; break;
    case 0x94: hit = w == 0xb || w == 0xc; break;
    case 0x96: hit = w == 0x18 || w == 0x19; break;
    case 0x9a: hit = w == 0x24 || w == 0x25; break;
    default: break;
    }
    if (!hit) return (a & ~0xffffu) | w;
    return e.h(0x695F0, e.rd32(ev + 0xc), 0, 0, 0, id, e.rd32(ev));
}
}
