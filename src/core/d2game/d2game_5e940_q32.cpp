// 1.13c D2Game+0x5E940 (EAX = unit [the opener], ECX = game; plain ret): quest 0x21 spawns its type-2 unit (class 0x22E) once. Walks the quest list (game+0x10F4 to the first node, chain +0xF4)
// for type 0x21; if active (+9) and its data (+0x18) flag +0xA4 is clear: allocates the unit (#11153 at 0xA6B8), fills it, registers it, allocates its 0x38-byte data block and
// marks the quest data (+0xA4 = 1, +0xA8 = unit id). On allocation failure it sends 0x8EA80 (code 7) and leaves the flag clear.
#include "d2game_q32.h"
using namespace q32;
static uint32_t M(uint32_t rva) { return 0xB2000000u | rva; }
// 1.13c D2Game+0x5E940
void d2game_5e940_q32(const Call& X, uint32_t unit, uint32_t game)
{
    const uint32_t ctl = rd32(game, 0x10f4);
    if (!ctl) return;
    uint32_t n = rd32(ctl, 0);
    if (!n) return;
    while (rd32(n, 0) != 0x21) { n = rd32(n, 0xf4); if (!n) return; }
    if (rd8(n, 9) == 0) return;
    const uint32_t st = rd32(n, 0x18);
    if (rd8(st, 0xa4) != 0) return;
    const uint32_t room = X(0xA478, 0, 0, 0, 0, 0, unit);
    const uint32_t r1130 = X(0x1130, 0, unit);
    const uint32_t r1100 = X(0x1100, 0, unit);
    const uint32_t pool = rd32(game, 0x1c);
    const uint32_t u = X(0xA6B8, 0, 0, 0, 0, 0, pool, 2);
    if (!u) {
        const uint32_t q = rd32(n, 4);
        X(0x8EA80, 0, q, unit, 0, 0, 7, rd32(q, 0xa8) + 0x19, 0, 0);
        return;
    }
    wr32(u, 0, 2); wr32(u, 4, 0x22e); wr32(u, 8, rd32(game, 0x1c)); wr32(u, 0x80, game);
    uint32_t f = rd32(u, 0xc8);
    f = rd32(game, 0x70) ? (f | 0x2000000) : (f & 0xfdffffffu);
    wr32(u, 0xc8, f);
    wr32(u, 0xc8, f | 0x4000000);
    const uint8_t act = (uint8_t)X(0xA56E, 0, 0, 0, 0, 0, X(0xA556, 0, 0, 0, 0, 0, room));
    wr8(u, 0x18, act);
    wr32(u, 0x1c, rd32(game, 0xbc + act * 4u));
    wr32(u, 0x74, 0);
    X(0xE0110, 0, 0, 0, u, game + 0xd0);   // the EBX slot carries EDI (the unit): the probe logs it as the J record
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x10);
    wr32(u, 0xd0, 0xb);
    uint32_t cnt = rd32(game, 0x98) + 1;
    if (cnt == 0xffffffffu) cnt = 1;
    wr32(game, 0x98, cnt);
    const uint32_t m = X(0xA2E6, 0, rd32(game, 0x1c), 0x38, 0, 0, M(0xFB02C), 0x265, 0);
    wr32(u, 0x14, m);
    for (uint32_t o = 0; o < 0x38; o += 4) wr32(m, o, 0);
    wr32(m, 0, X(0xA520, 0, 0, 0, 0, 0, 0x22e));
    wr32(u, 0x10, 0);
    X(0xA724, 0, 0, 0, 0, 0, u);
    X(0x2F300, 0, 0, 0, 0, u, cnt, room, r1100, r1130);
    X(0xE0F40, room, r1130, u, 0, 0, r1100, game, 1);
    wr32(u, 0xc4, rd32(u, 0xc4) | 0x3000000);
    wr8(st, 0xa4, 1);
    wr32(st, 0xa8, rd32(u, 0xc));
}
