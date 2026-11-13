// 1.13c D2Game+0x6B930 -- unit event (fastcall ECX = game, EDX = unit): the object 0xA478(unit) has a 0xA556 type t; a unit of kind 0x7D (t 0x6F), 0x7E (t 0x70) or 0x7F is placed by
// 0xA22E0(game, unit, kind, t). The placed unit's 0xAEE0 byte | 3 replaces the low byte of the saved ECX (the game pointer; its upper bytes ride along) in 0xAEC8(placed, word).
// Then 0xA51A(0xA478(unit), 0) and 0xA51A(0xA478(placed), 0), the second also when nothing was placed. Void.
#include <cstdint>
#include "../fn/d2game_nm2.h"
namespace d2g {
void f6b930(NE& e, uint32_t game, uint32_t unit)
{
    const uint32_t t = e.imp(0xA556, e.imp(0xA478, unit));
    const uint32_t kind = t == 0x6f ? 0x7d : t == 0x70 ? 0x7e : 0x7f;
    const uint32_t u = e.imp(0xA22E0, game, unit, kind, t);
    if (u) {
        const uint32_t al = (uint8_t)e.imp(0xAEE0, u) | 3u;
        e.imp(0xAEC8, u, (game & ~0xFFu) | al);
    }
    e.imp(0xA51A, e.imp(0xA478, unit), 0);
    e.imp(0xA51A, e.imp(0xA478, u), 0);
}
}
