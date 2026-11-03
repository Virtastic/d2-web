// D2Game+0x716D0 -- strips a leaving unit's inventory and announces it: stack game, unit (ret 8). Written from the 1.13c disassembly; callees are the D2Seam.
// 0xA4C0(unit): the player data; +0x5C clear returns. The client is the player data +0x9C (unit type 0 only, else 0); the id is unit+0xC (-1 for a null unit).
// Packet {0x92, 0, id} (6 bytes) goes to 0x8A3E0(client). 0xDFC10 (EBX callback 0x70C90, EDI = game, stack unit). Then every inventory item
// (0xA3DC first, 0xA46C node, 0xA4CC next, the next read before the node is touched) whose node has type 4: 0xD2BE0 (EAX = node, EDI = unit) when
// node+0x10 is clear, 0xA994(inv, node), 0xA916(node, 1, 1). Then the 0xA6A6 item with the same treatment (no 0xD2BE0), 0xD5590 (EAX = unit, stack game) and
// 0x715E0 (EAX = unit, ECX = game).
#include "d2game_seam4e.h"
// 1.13c D2Game+0x716D0
extern "C" void d2game_716d0(uint32_t game, uint32_t unit, const D2Seam* e)
{
    const uint32_t pd = e->call(0xA4C0, 0, 0, 0, 0, 0, unit);
    if (!d2rd32(pd + 0x5C)) return;
    uint32_t client = 0, id = 0xFFFFFFFFu;
    if (unit) {
        id = d2rd32(unit + 0xC);
        if (d2rd32(unit) == 0) client = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C);
    }
    uint32_t pk[2] = { 0x92u | (id << 16), id >> 16 };
    e->call(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)pk, 6);
    e->blk(0x8A3E0, pk, 2);
    e->call(0xDFC10, 0, 0, 0, 0xB2000000u | 0x70C90, game, unit);
    uint32_t item = e->call(0xA3DC, 0, 0, 0, 0, 0, d2rd32(unit + 0x60));
    while (item) {
        const uint32_t node = e->call(0xA46C, 0, 0, 0, 0, 0, item);
        const uint32_t next = e->call(0xA4CC, 0, 0, 0, 0, 0, item);
        if (node && d2rd32(node) == 4) {
            if (!d2rd32(node + 0x10)) e->call(0xD2BE0, node, 0, 0, 0, unit);
            e->call(0xA994, 0, 0, 0, 0, 0, d2rd32(unit + 0x60), node);
            e->call(0xA916, 0, 0, 0, 0, 0, node, 1, 1);
        }
        item = next;
    }
    const uint32_t last = e->call(0xA6A6, 0, 0, 0, 0, 0, d2rd32(unit + 0x60));
    if (last && d2rd32(last) == 4) {
        e->call(0xA994, 0, 0, 0, 0, 0, d2rd32(unit + 0x60), last);
        e->call(0xA916, 0, 0, 0, 0, 0, last, 1, 1);
    }
    e->call(0xD5590, unit, 0, 0, 0, 0, game);
    e->call(0x715E0, unit, game);
}
