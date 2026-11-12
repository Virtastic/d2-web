// 1.13c D2Game+0xA8430: monster skill-use step for the "strike target" action. ECX = game, EDX = unit (ESI throughout), one stack word p3 (ret 4).
// Written from the 1.13c disassembly as a rule over an environment E (hookenv.h): every callee that is not a field read is one E::h(rva, ...) call.
// p3 = {ctx +0, target +8, skill record +0x1C}; ctx = {+0x14 mode, +0x18 flags, +0x1C stamp}. A null skill record or a stamp in the future ends with
// 0xF22C0 (EAX = game; stack 10 or the signed stamp gap). Otherwise 0xB17B0 (EAX 3, ECX = 4-dword block {0, 0x40, found, 0x40}, EDX = game; unit, 0)
// fills block[2]. A found entry tests the skill (0x1420 row, two 0xA68E checks, a 0x1180 roll against the skill's per-difficulty chance) and casts
// through 0xF2380; failure is 0xF22C0(10). No entry: flags bit 0 set casts the 5-entry table row (mode below 5) or the 0x22F effect (mode 5 and up);
// bit 0 clear starts the wind-up (tables 0x1830 / 0xADE4, 0x7A090 notes, 0xF2380, flags |= 1, stamp = game+0xA8 + 250, 0xDFFD0(0x10, 0)).
// The return value is EAX at the ret: the last callee's result on every exit, except a null 0xADE4 node (0) and a negative node kind (the node).
#pragma once
#include <cstdint>

namespace ra8430 {
// tab = the 5-dword table at D2Game+0xFB688 (a module global; the probe pins it), gdt = sgptDataTables (D2Game+0xF829C) read by the caller.
template <class E> uint32_t Step(E& e, uint32_t game, uint32_t unit, uint32_t p3, uint32_t tab, uint32_t dataTables)
{
    const uint32_t skill = e.rd32(p3 + 0x1C);
    if (!skill) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
    const uint32_t ctx = e.rd32(p3);
    const uint32_t gap = e.rd32(game + 0xA8);
    if ((int32_t)gap < (int32_t)e.rd32(ctx + 0x1C)) return e.h(0xF22C0, game, 0, 0, 0, 0, gap - e.rd32(ctx + 0x1C));
    const uint32_t mode = e.rd32(ctx + 0x14);
    uint32_t blk[4] = { 0, 0x40, 0, 0x40 };
    uint32_t eax = e.h(0xB17B0, 3, e.stk(blk), game, 0, 0, unit, 0);
    if (blk[2]) {
        const uint32_t target = e.rd32(p3 + 8);
        const int32_t sid = e.rd16s(skill + 0x170);
        if (sid <= 0 || !target) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
        const uint32_t row = e.h(0x1420, (uint32_t)sid);
        if (!row) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
        const int32_t w0 = e.rd16s(row + 0x80);
        if (!(w0 < 0 || w0 >= (int32_t)e.rd32(e.rd32(dataTables) + 0xC4)))
            if (e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)w0)) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
        const int32_t w1 = e.rd16s(row + 0x82);
        if (!(w1 < 0 || w1 >= (int32_t)e.rd32(e.rd32(dataTables) + 0xC4)))
            if (e.h(0xA68E, 0, 0, 0, 0, 0, target, (uint32_t)w1)) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
        const uint32_t roll = e.h(0x1180, 100, unit + 0x20);
        if ((int32_t)roll >= e.rd16s(skill + 0x56 + e.rd8(game + 0x6D) * 2)) return e.h(0xF22C0, game, 0, 0, 0, 0, 10);
        return e.h(0xF2380, (target & 0xFFFFFF00u) | e.rd8(skill + 0x180), unit, 0, 0, 0, (uint32_t)e.rd16s(skill + 0x170), target, 0, 0);
    }
    const uint32_t flags = e.rd32(ctx + 0x18);
    if (flags & 1) {
        if ((int32_t)mode < 5) {
            const uint32_t id = e.h(0x1830, e.rd32(tab + mode * 4));
            const uint32_t r = e.h(0xADF6, 0, 0, 0, 0, 0, 2, id);
            return e.h(0xA4E40, 0, 0, 0, 0, 0, r);
        }
        e.h(0xAFC50, unit, 0, 0, 0, 0, game, 0x22F, 1);
        const uint32_t v = e.h(0x18E0, unit);
        e.h(0x9D230, 0, game, unit, 0, 0, v, 0);
        e.h(0xA3CA, 0, 0, 0, 0, 0, unit, 0x8E, 1);
        const uint32_t type = unit ? e.rd32(unit) : 6, id = unit ? e.rd32(unit + 0xC) : 0xFFFFFFFFu;
        const uint32_t u2 = e.h(0xA6A0, 0, 0, 0, 0, 0, e.rd32(game + 0x1C), 0, 0, type, id);
        if (u2) {
            e.h(0xA60A, 0, 0, 0, 0, 0, unit, u2, 1);
            e.h(0xA5B6, 0, 0, 0, 0, 0, u2, 0x8E);
            e.h(0xA6E2, 0, 0, 0, 0, 0, u2, 0x163, 0x22F, 0);
        }
        return e.h(0xF22C0, game, 0, 0, 0, 0, 5);
    }
    eax = ctx;   // EAX is [p3] here and survives to the 0xF2380 call unless a callee replaces it
    if ((int32_t)mode >= 0 && (int32_t)mode < 5) {
        const uint32_t id = e.h(0x1830, e.rd32(tab + mode * 4));
        const uint32_t node = e.h(0xADE4, 0, 0, 0, 0, 0, id);
        if (!node) return 0;
        const int32_t kind = (int32_t)e.rd32(node + 4);
        if (kind < 0) return node;
        eax = e.h(0x7A090, 0, 0, 0, 0, unit, (uint32_t)kind);
        if (kind == 0x3E) eax = e.h(0x7A090, 0, 0, 0, 0, unit, 0x17);
        else if (kind == 0x69) eax = e.h(0x7A090, 0, 0, 0, 0, unit, 0x17D);
    }
    e.h(0xF2380, (eax & 0xFFFFFF00u) | 0x0A, unit, 0, 0, 0, 0x11D, unit, 0, 0);
    e.wr32(ctx + 0x18, e.rd32(ctx + 0x18) | 1);
    e.wr32(ctx + 0x1C, e.rd32(game + 0xA8) + 0xFA);
    return e.h(0xDFFD0, 0, 0, 0, 0, 0, 0x10, 0);
}
}
