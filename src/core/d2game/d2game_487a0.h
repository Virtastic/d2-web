// 1.13c D2Game+0x487A0 (register a monster in an object's pending-node list and tell the client), transcribed from the 1.13c
// disassembly. Convention: EAX = unit (kept in ESI), stack = (game, obj, &head, word), ret 0x10. Results: 1 when 0x15D0(obj+4, 9) is
// zero or the unit is already in the list; 0 otherwise (incl. every early exit). Order: when the byte unit+0x6C is set, 0xE03A0
// (EAX = unit+0x68, ECX = game, EDX = unit+0x64) non-zero returns 0; obj null, obj+0x10 zero or 0xC, or 0x37680 (ESI = unit) == 1 return 0;
// 0x15D0 (EAX = obj+4, EDX = 9); when obj+4 is 0x92, 0x60390 (EAX = game) non-zero returns 0; a node with node+0 == unit in the list at
// *head returns 1. Client record = [0xA4C0(unit) + 0x9C] when the unit's first dword is 0, else 0. A 12-byte node (Fog alloc 0xA2E6:
// ECX = [game+0x1C], EDX = 0xC, stack file string, 0x1C7, 0) takes node+0 = unit, node+8 = old head and becomes the head; then
// 0x49A80 (EAX = client), 0xBD330 (EAX = obj, stack that result, client; ret 8), 0xDFAE0 (EAX = unit, stack 1, obj+0xC; ret 8),
// node+4 = 0, handle = 0xB210([game+0x1C]), 0x34660 (EAX = client, stack unit, obj, handle; ret 0xC), 0xB20A (stack handle, a pointer
// 6 bytes past the packet), packet {0x27, byte obj+0, obj+0xC} through 0x8B3D0 (EAX = packet, stack client; ret 4; the seventh byte on is
// stack residue, the capture keeps only the first word), 0xB216 (handle), 0x35030 (EAX = game, stack client), 0x1020 (EAX = unit, stack game;
// ret 4 leaves the word on the stack), and 0x8AD40 (AL = 0x28, CL = 1, EDX = obj+0xC, stack client, the 0x1020 result, word; ret 0xC).
#pragma once
#include <cstdint>

namespace r487a0 {
template <class E> uint32_t Do(E& e, uint32_t unit, uint32_t game, uint32_t obj, uint32_t headp, uint32_t w3)
{
    uint32_t head = e.rd32(headp);
    if (e.rd8(unit + 0x6C)) {
        if (e.h(0xE03A0, e.rd32(unit + 0x68), game, e.rd32(unit + 0x64))) return 0;
    }
    if (!obj) return 0;
    const uint32_t t = e.rd32(obj + 0x10);
    if (!t || t == 0xC) return 0;
    if (e.h(0x37680, t, 0, 0, 0, unit) == 1) return 0;
    const uint32_t kind = e.rd32(obj + 4);
    if (!e.h(0x15D0, kind, 0, 9)) return 1;
    if (kind == 0x92 && e.h(0x60390, game)) return 0;
    for (uint32_t n = head; n; n = e.rd32(n + 8)) if (e.rd32(n) == unit) return 1;
    const uint32_t client = e.rd32(unit) == 0 ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, unit) + 0x9C) : 0;
    const uint32_t pool = e.rd32(game + 0x1C);
    const uint32_t node = e.h(0xA2E6, 0, pool, 0xC, 0, 0, 0xB20FC358u, 0x1C7, 0);
    e.wr32(node, unit);
    const uint32_t old = e.rd32(headp);
    e.wr32(node + 8, old);
    e.wr32(headp, node);
    const uint32_t r49 = e.h(0x49A80, client);
    e.h(0xBD330, obj, 0, 0, 0, 0, r49, client);
    const uint32_t id = e.rd32(obj + 0xC);
    e.h(0xDFAE0, unit, 0, 0, 0, 0, 1, id);
    e.wr32(node + 4, 0);
    const uint32_t hdl = e.h(0xB210, 0, 0, 0, 0, 0, pool);
    e.h(0x34660, client, 0, 0, 0, 0, unit, obj, hdl);
    uint32_t buf[2] = { 0x27u | ((e.rd32(obj) & 0xFFu) << 8) | (id << 16), id >> 16 };
    e.h(0xB20A, 0, 0, 0, 0, 0, hdl, e.stk((const char*)buf + 6));
    e.h(0x8B3D0, e.stk(buf), 0, 0, 0, 0, client);
    e.blk(0x8B3D0, buf, 2, 1);
    e.h(0xB216, 0, 0, 0, 0, 0, hdl);
    e.h(0x35030, game, 0, 0, 0, 0, client);
    const uint32_t r10 = e.h(0x1020, unit, 0, 0, 0, 0, game);
    e.h(0x8AD40, (r10 & ~0xFFu) | 0x28u, 0, id, 0, 0, client, r10, w3);
    return 0;
}
}
