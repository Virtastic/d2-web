// 1.13c D2Game+0x663A0 -- ECX = game; no result. Written from the 1.13c disassembly.
// The type-4 quest's data (+0x18) gets +0x91 = +0x52 = 1. With +0x70 == 1 the unit with id +0x6C (hash +0x1520, chain +0xE4)
// is packed into ctx {game, unit, 0xA478(unit), ?, ?, 0x1100(ECX unit), 0x1130(ECX unit)} for 0x66320(ECX = &ctx). Then with
// +0x51 == 1 the object with id +0x68 (hash +0x1320) gives its position (0xA544(obj, &pt)) and room (0xA478(obj)); a room from
// 0x1280(EAX room, EBX pt.x, EDI pt.y) spawns through 0xE1D90(ECX 2, EDX 0xBD; pt.x, pt.y, game, room, 1, 1, 0); a unit sets
// +0x96 = 1 and +0xA4 = its id.
#include "../fn/hookenv.h"
namespace d2g {
void f663a0(HookEnv& e, uint32_t game)
{
    uint32_t q = e.rd32(game + 0x10F4);
    if (!q) return;
    q = e.rd32(q);
    while (q && e.rd32(q) != 4) q = e.rd32(q + 0xF4);
    if (!q) return;
    const uint32_t d = e.rd32(q + 0x18);
    e.wr8(d + 0x91, 1);
    e.wr8(d + 0x52, 1);
    if (e.rd8(d + 0x70) != 1) return;
    const uint32_t id = e.rd32(d + 0x6C);
    uint32_t u = e.rd32(game + 0x1520 + (id & 0x7F) * 4);
    while (u && e.rd32(u + 0xC) != id) u = e.rd32(u + 0xE4);
    if (!u) return;
    uint32_t ctx[7] = { game, u, 0, 0, 0, 0, 0 };
    ctx[2] = e.h(0xA478, 0, 0, 0, 0, 0, u);
    ctx[5] = e.h(0x1100, 0, u);
    ctx[6] = e.h(0x1130, 0, u);
    e.h(0x66320, 0, e.stk(ctx));
    e.blk(0x66320, ctx, 7, 0x67);
    if (e.rd8(d + 0x51) != 1) return;
    const uint32_t oid = e.rd32(d + 0x68);
    uint32_t o = e.rd32(game + 0x1320 + (oid & 0x7F) * 4);
    while (o && e.rd32(o + 0xC) != oid) o = e.rd32(o + 0xE4);
    if (!o) return;
    uint32_t pt[2] = { 0, 0 };
    e.h(0xA544, 0, 0, 0, 0, 0, o, e.stk(pt));
    const uint32_t rm = e.h(0xA478, 0, 0, 0, 0, 0, o);
    const uint32_t room = e.h(0x1280, rm, 0, 0, pt[0]);
    e.edi(0x1280, pt[1]);
    if (!room) return;
    const uint32_t args[7] = { pt[0], pt[1], game, room, 1, 1, 0 };
    const uint32_t r = e.h(0xE1D90, 0, 2, 0xBD, 0, 0, args[0], args[1], args[2], args[3], args[4], args[5], args[6]);
    e.blk(0xE1D90, args, 7);
    if (!r) return;
    e.wr8(d + 0x96, 1);
    e.wr32(d + 0xA4, e.rd32(r + 0xC));
}
}
