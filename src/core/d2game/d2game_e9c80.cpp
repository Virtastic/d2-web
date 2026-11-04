// D2Game Gidbinn decoy object operate function over retail-layout (32-bit) memory. Written from the 1.13c disassembly.
// Convention: ECX = operate block (+0 game, +4 object, +8 player, +0x10 unit id); returns 0. A busy object (+0x10 mode set) is left alone.
// Quest flags 0x13 (bits 0, 7, 8) on the player's per-difficulty record send message 0x13 to the player; else the quest node
// type 0x11 in the game's quest list (+0x10F4, node +0xF4 next) is woken, the object put in mode 1 and its event scheduled.
#include "d2game_seam4e.h"

static inline uint32_t mod(uint32_t rva) { return 0xB2000000u | rva; }

// 1.13c D2Game+0xE9C80
extern "C" uint32_t d2game_e9c80(uint32_t op, const D2Seam* e)
{
    const uint32_t obj = d2rd32(op + 4);
    if (obj && d2rd32(obj + 0x10)) return 0;
    static const uint32_t bits[3] = { 0, 7, 8 };
    for (uint32_t b : bits) {
        const uint32_t game = d2rd32(op + 0);
        const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, d2rd32(op + 8));
        const uint32_t rec = d2rd32(pd + 0x10 + 4 * d2rd8(game + 0x6D));
        if (e->call(0xA568, 0, 0, 0, 0, 0, rec, 0x13, b)) {
            const uint32_t pl = d2rd32(op + 8);
            e->call(0xDFFD0, 0, 0, 0, 0, pl, 0x13, pl);
            return 0;
        }
    }
    const uint32_t ql = d2rd32(d2rd32(op + 0) + 0x10F4);
    if (!ql) return 0;
    uint32_t node = d2rd32(ql);
    while (node && d2rd32(node) != 0x11) node = d2rd32(node + 0xF4);
    if (!node || !d2rd8(node + 9)) return 0;
    if (!d2rd8(node + 0xC)) d2wr8(node + 0xC, 1);
    const uint32_t ext = d2rd32(node + 0x18);
    e->call(0xA4A2, 0, 0, 0, 0, 0, d2rd32(op + 4), 1);
    const uint32_t u = e->call(0xA520, 0, 0, 0, 0, 0, d2rd32(op + 0x10));
    const uint32_t game = d2rd32(op + 0);
    const uint32_t w[5] = { 1, (d2rd32(u + 0xDC) >> 8) + d2rd32(game + 0xA8), 0, 0, 0 };
    e->call(0x8EA80, 0, game, d2rd32(op + 4), 0, 0, w[0], w[1], w[2], w[3], w[4]);
    e->blk(0x8EA80, w, 5);
    d2wr8(ext + 3, 1);
    if (!d2rd8(ext + 5)) {
        d2wr8(ext + 5, 1);
        e->call(0x33AF0, 0, 0, 0, 7, 0, node, mod(0xE8F30));
    }
    return 0;
}
