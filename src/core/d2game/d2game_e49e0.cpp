// 1.13c D2Game+0xE49E0 -- ECX = quest node, EDX = event (+0 game, +0xC unit); no stack argument. With the unit's quest-flag buffer for the game's difficulty (0xA4C0 player data, byte +0x6D, buffers from +0x10): a non-zero 0xA568(buffer, 0x12, 0) is returned untouched; a non-zero 0xA568(buffer, 0x12, 7) sets the node's +0xC byte to 2 and +0xB byte to 7 and is returned; otherwise a non-zero 0xA568(buffer, 0x12, 2) sets those same bytes, and then the node's +0x18 record is read: when the sum of its words +0x10, +0x14, +0x18, +0x1C is non-zero the bytes are set as well. Returns the +0x18 record pointer on that last path.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe49e0(GE& e, uint32_t node, uint32_t ev)
{
    const uint32_t game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    uint32_t v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x12, 0);
    if (v) return v;
    v = e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x12, 7);
    if (v) { e.wr8(node + 0xc, 2); e.wr8(node + 0xb, 7); return v; }
    if (e.h(0xA568, 0, 0, 0, 0, 0, buf, 0x12, 2)) { e.wr8(node + 0xc, 2); e.wr8(node + 0xb, 7); }
    const uint32_t d = e.rd32(node + 0x18);
    if (e.rd32(d + 0x1c) + e.rd32(d + 0x18) + e.rd32(d + 0x14) + e.rd32(d + 0x10) == 0) return d;
    e.wr8(node + 0xc, 2); e.wr8(node + 0xb, 7);
    return d;
}
}
