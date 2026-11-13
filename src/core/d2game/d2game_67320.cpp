// 1.13c D2Game+0x67320 -- quest callback (fastcall ECX = game, EDX = unit; one unused stack word): the object 0xA478(unit) must have 0xA556 type 0x25, else 0. The unit's position (0xA544 pair) goes to
// 0xDFFD0(0x18, unit; ESI = unit) when 0xA808 holds. Otherwise the pair is moved by 0xAE98(obj, in, out, 3, 0x3E01, 0xC01, 0) in place (0xA56E(1) and 0xA56E(type) must agree, else retail aborts),
// a room 0x1280(obj) is looked up and 0xE1D90(ECX 2, EDX 0x3B; x, y, game, room, 1, 1, 0) builds a unit that 0xA22E0(game, it, 1, type) places; the unit's 0xAEE0 byte | 3 joins the upper bytes of the
// type word into 0xAEC8(unit, word); 0xA51A(room, 0) and 0xA51A(0xA478(unit), 0). Returns 1 past the first guard, else 0.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
uint32_t f67320(NE& e, uint32_t game, uint32_t unit)
{
    const uint32_t obj = e.imp(0xA478, unit);
    if (!obj) return 0;
    if (e.imp(0xA556, obj) != 0x25) return 0;
    uint32_t pt[2] = {};
    e.imp(0xA544, unit, e.stk(pt));
    const uint32_t x = pt[0], y = pt[1];
    if (unit && e.imp(0xA808, obj)) {
        e.h(0xDFFD0, 0, 0, 0, 0, unit, 0x18, unit);
        return 1;
    }
    const uint8_t b1 = (uint8_t)e.imp(0xA56E, 1);
    uint32_t type = e.imp(0xA556, obj);
    const uint8_t b2 = (uint8_t)e.imp(0xA56E, type);
    if (b1 != b2) return 1;   // retail logs an assert and exits here
    uint32_t blk[2] = { x, y };
    const uint32_t pre[2] = { x, y };
    const uint32_t ok = e.imp(0xAE98, obj, e.stk(blk), e.stk(blk), 3, 0x3E01, 0xC01, 0);
    e.blk(0xAE98, pre, 2);
    if (!ok) return 1;
    const uint32_t nx = blk[0], ny = blk[1];
    const uint32_t room = e.h(0x1280, obj);
    if (!room) return 1;
    uint32_t u = e.h(0xE1D90, 0, 2, 0x3b, 0, 0, nx, ny, game, room, 1, 1, 0);
    e.imp(0xA4A2, u, 1);
    u = e.imp(0xA22E0, game, u, 1, type);
    if (!u) return 1;
    const uint8_t t = (uint8_t)e.imp(0xAEE0, u);
    type = (type & ~0xFFu) | (uint32_t)(uint8_t)(t | 3);
    e.imp(0xAEC8, u, type);
    e.imp(0xA51A, room, 0);
    e.imp(0xA51A, e.imp(0xA478, u), 0);
    return 1;
}
}
