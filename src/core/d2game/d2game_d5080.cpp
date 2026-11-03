// D2Game+0xD5080 -- sends every inventory item of a unit as an item message: EAX = unit, stack a0 (game), a1, a2 (a unit or null), a3 (ret 0x10). Written from the
// 1.13c disassembly; callees are the D2Seam. 0xA9EE(unit) zero returns 1; unit+0x60 null returns 0. Per item (0xA3DC first, 0xA4CC next): 0xA46C node, 0x107E0
// (EAX = a0, node, 1) builds the message, 0xA4A2(msg, 4), the node's two position values (types 2, 4, 5: [node+0x2C]+0xC / +0x10; else 0, or 0xA454 / 0xA3B8 of
// [node+0x2C]) go to 0xA8F2 / 0xA964 (msg, value), then 0xD4A70 (EAX = msg id or -1; stack a0, a1, id of a2 or -1, &args block, a3, 0, 1, 0): zero returns 0.
// All items sent returns 1.
#include "d2game_seam4e.h"
static uint32_t d5080_pos(const D2Seam* e, uint32_t node, uint32_t off, uint32_t rva)
{
    const uint32_t t = d2rd32(node), p = d2rd32(node + 0x2C);
    if (t == 2 || (int32_t)t > 3 && (int32_t)t < 6) return d2rd32(p + off);
    return p ? e->call(rva, 0, 0, 0, 0, 0, p) : 0;
}
// 1.13c D2Game+0xD5080
extern "C" uint32_t d2game_d5080(uint32_t unit, uint32_t a0, uint32_t a1, uint32_t a2, uint32_t a3, const D2Seam* e)
{
    if (!e->call(0xA9EE, 0, 0, 0, 0, 0, unit)) return 1;
    const uint32_t inv = d2rd32(unit + 0x60);
    if (!inv) return 0;
    uint32_t blkw[4] = { a0, a1, a2, a3 };
    for (uint32_t item = e->call(0xA3DC, 0, 0, 0, 0, 0, inv); item;) {
        const uint32_t node = e->call(0xA46C, 0, 0, 0, 0, 0, item);
        const uint32_t msg = e->call(0x107E0, a0, 0, 0, 0, 0, node, 1);
        e->call(0xA4A2, 0, 0, 0, 0, 0, msg, 4);
        e->call(0xA8F2, 0, 0, 0, 0, 0, msg, d5080_pos(e, node, 0xC, 0xA454));
        e->call(0xA964, 0, 0, 0, 0, 0, msg, d5080_pos(e, node, 0x10, 0xA3B8));
        const uint32_t id2 = a2 ? d2rd32(a2 + 0xC) : 0xFFFFFFFFu;
        const uint32_t idm = msg ? d2rd32(msg + 0xC) : 0xFFFFFFFFu;
        if (!e->call(0xD4A70, idm, 0, 0, 0, 0, a0, a1, id2, (uint32_t)(uintptr_t)blkw, a3, 0, 1, 0)) return 0;
        item = e->call(0xA4CC, 0, 0, 0, 0, 0, item);
    }
    return 1;
}
