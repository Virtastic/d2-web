// D2Game+0xD5590 -- strips a unit's inventory of items that no longer belong: EAX = unit, stack game (ret 4). Written from the 1.13c disassembly; callees are the D2Seam.
// inv = unit+0x60 (null: return). Walks 0xA3DC (first item) / 0xA4CC (next item, read before the item is touched). Per item: 0xA46C (node; null is fatal 0x19FE),
// 0xA736 (item type): type 3, or type 1 with 0xA640(node, unit) set, runs 0xA8AA(unit, node), 0xA946(inv, node), 0xA748(inv, node) (null is fatal 0x1A10, a
// different node is fatal 0x1A11); type 3 again also 0xA5C2(inv, node) then 0xA742(inv, that); then 0xD46B0(game, 0, 1) and 0xD51A0(game, unit, 0). Ends with 0xA880(inv).
// The fatal paths print through 0xA2DA / 0xA2C2 and exit the process (0x2A3C with -1).
#include "d2game_seam4e.h"
// 1.13c D2Game+0xD5590
extern "C" void d2game_d5590(uint32_t unit, uint32_t game, const D2Seam* e)
{
    if (!unit) return;
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return;
    auto fatal = [&](uint32_t code) {
        const uint32_t s = e->call(0xA2DA, 0, 0, 0, 0, 0, code);
        e->call(0xA2C2, 0, 0, 0, 0, 0, 0, s);
        e->call(0x2A3C, 0, 0, 0, 0, 0, 0xFFFFFFFFu);
    };
    uint32_t item = e->call(0xA3DC, 0, 0, 0, 0, 0, inv);
    while (item) {
        const uint32_t next = e->call(0xA4CC, 0, 0, 0, 0, 0, item);
        const uint32_t node = e->call(0xA46C, 0, 0, 0, 0, 0, item);
        if (!node) { fatal(0x19FE); return; }
        if (e->call(0xA736, 0, 0, 0, 0, 0, item) == 3 ||
            (e->call(0xA736, 0, 0, 0, 0, 0, item) == 1 && e->call(0xA640, 0, 0, 0, 0, 0, node, unit) != 0)) {
            e->call(0xA8AA, 0, 0, 0, 0, 0, unit, node);
            e->call(0xA946, 0, 0, 0, 0, 0, inv, node);
            const uint32_t r = e->call(0xA748, 0, 0, 0, 0, 0, inv, node);
            if (!r) { fatal(0x1A10); return; }
            if (node != r) { fatal(0x1A11); return; }
            if (e->call(0xA736, 0, 0, 0, 0, 0, item) == 3) {
                const uint32_t x = e->call(0xA5C2, 0, 0, 0, 0, 0, inv, node);
                e->call(0xA742, 0, 0, 0, 0, 0, inv, x);
            }
            e->call(0xD46B0, 0, 0, 0, 0, 0, game, 0, 1);
            e->call(0xD51A0, 0, 0, 0, 0, 0, game, unit, 0);
        }
        item = next;
    }
    e->call(0xA880, 0, 0, 0, 0, 0, inv);
}
