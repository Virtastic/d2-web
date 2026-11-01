// 1.13c D2Game+0x48410 (flush a monster's pending-node list: tell the client, reset each node's object, release the node),
// transcribed from the 1.13c disassembly. Convention: stack = (game, unit, callback), ret 0xC; EAX is left alone on the null-unit exit
// (the probe carries it in). The unit must be non-null, type 1, with a data pointer (unit+0x14) whose +0x30 is a list header; the header's
// first dword is the first node (node+0 object, node+8 next). Early exits return EAX-in, the type, 0, 0. Per node, taking next = node+8
// first: when the object is non-null and its first dword is 0, the client record is [0xA4C0(object) + 0x9C] (else 0); the 7-byte packet
// {0x62, 1, unit id (unit+0xC)} goes through 0x8A3E0 (EAX = client record, stack buffer, 7; ret 8) -- its last byte is stack residue and
// the capture logs only the first buffer word; object+0x64 = -1, +0x68 = 6, byte +0x6C = 0; the callback (when given) is called with
// ECX = object; then 0x0A2EC (ECX = [game+0x1C], EDX = node, stack file string, 0x363, 0; ret 0xC) frees the node. After the list is
// walked the header's first dword is set to 0 and the header address is returned.
#pragma once
#include <cstdint>

namespace r48410 {
template <class E, class F> uint32_t Do(E& e, uint32_t eaxIn, uint32_t game, uint32_t unit, uint32_t cb, F&& callCb)
{
    if (!unit) return eaxIn;
    const uint32_t type = e.rd32(unit);
    if (type != 1) return type;
    const uint32_t mon = e.rd32(unit + 0x14);
    if (!mon) return 0;
    const uint32_t list = e.rd32(mon + 0x30);
    if (!list) return 0;
    for (uint32_t node = e.rd32(list); node;) {
        const uint32_t obj = e.rd32(node), next = e.rd32(node + 8), id = e.rd32(unit + 0xC);
        const uint32_t client = (obj && e.rd32(obj) == 0) ? e.rd32(e.h(0xA4C0, 0, 0, 0, 0, 0, obj) + 0x9C) : 0;
        uint32_t buf[2] = { 0x62u | (1u << 8) | (id << 16), id >> 16 };
        e.h(0x8A3E0, client, 0, 0, 0, 0, e.stk(buf), 7);
        e.blk(0x8A3E0, buf, 2, 1);   // the packet's seventh byte is stack residue: only its first word is part of the call
        e.wr32(obj + 0x64, 0xFFFFFFFFu);
        e.wr32(obj + 0x68, 6);
        e.wr8(obj + 0x6C, 0);
        if (cb) callCb(obj);
        e.h(0xA2EC, 0, e.rd32(game + 0x1C), node, 0, 0, 0xB20FC358u, 0x363, 0);
        node = next;
    }
    e.wr32(list, 0);
    return list;
}
}
