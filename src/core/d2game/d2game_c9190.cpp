// 1.13c D2Game+0xC9190 -- Act 2 quest event callback (ECX = quest node, EDX = event {+0 game, +0xC player unit, +0x14 and +0x18 event ids}); written from the 1.13c disassembly.
// Event +0x18 = 0x4A: a node state (+0xC) below 4 becomes 4 (debug log 0xCBE70 around it when the global dbg is set); a step (+0xB) below 4 becomes 4 with +0x14 cleared. A changed
// state or step registers C5030 (step) and C3550 through 0xDFCC0 (EDI = game); nothing changed returns the old state. Event +0x18 = 0x32: flags (group 0xC, 8 and 7) of the player
// are set through 0xA57A. Event +0x14 = 0x28 (node data = node +0x18): data byte +0xC = 1 looks up the unit under data +0x3C in the game's table at +0x1320 (hash &0x7F, chain
// at +0xE4): a hit with a non-empty list (0x48370 on the unit's +0x14 block +0x30 when a type 1 unit) runs 0x48410(game, unit, 0), else 0x2CD10 releases it; the byte then clears
// (kept when 0x48410 ran). Data bytes +0xD clear and +0x11 set look up the unit under data +0x38 in the table at +0x1520: hit runs 0xA544(unit, buf), 0xA478(unit) and
// 0xC8FD0(node, id; ESI buf). A node word +0x9C then runs the party broadcast 0x33830. A player with flag (0xC, 0) or (0xC, 1) returns 1, else a node in state 3 runs 0x35EB0(EAX 4)
// and registers C3550. A node with no game (retail exit) is not modelled.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t fc9190(HookEnv& e, uint32_t node, uint32_t ev, uint32_t dbg)
{
    const uint32_t k18 = e.rd32(ev + 0x18), k14 = e.rd32(ev + 0x14);
    const uint32_t game0 = e.rd32(node + 4);
    uint32_t eax = k18;
    if (k18 == 0x4A) {
        bool ch = false;
        eax = e.rd8(node + 0xC);
        if (e.rd8(node + 0xC) < 4) {
            ch = true;
            e.wr8(node + 0xC, 4);
            if (dbg) { char buf[128]; e.h(0xCBE70, 0, 0, e.stk(buf), 0, 0, game0, 4); }
        }
        if (e.rd8(node + 0xB) < 4) {
            e.wr8(node + 0x14, 0);
            e.wr8(node + 0xB, 4);
            e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC5030, 0, 0); e.edi(0xDFCC0, game0);
        } else if (!ch) return eax;
        eax = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC3550, 0, 0); e.edi(0xDFCC0, e.rd32(node + 4));
        return eax;
    }
    if (k18 == 0x32) {
        const uint32_t pl = e.rd32(ev + 0xC);
        for (uint32_t n = 8; n >= 7; --n) {
            const uint32_t g = e.rd32(node + 4);
            const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
            eax = e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(pd + e.rd8(g + 0x6D) * 4 + 0x10), 0xC, n);
        }
    }
    if (k14 != 0x28) return eax;
    const uint32_t d = e.rd32(node + 0x18);
    if (e.rd8(d + 0xC) == 1) {
        const uint32_t g = e.rd32(node + 4), key = e.rd32(d + 0x3C);
        uint32_t u = e.rd32(g + 0x1320 + (key & 0x7F) * 4);
        while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
        if (u) {
            uint32_t a = 0;
            if (e.rd32(u) == 1 && e.rd32(u + 0x14)) a = e.rd32(e.rd32(u + 0x14) + 0x30);
            if (e.h(0x48370, a)) { e.h(0x48410, 0, 0, 0, 0, 0, g, 0, 0); }
            else { e.h(0x2CD10, 0); e.wr8(d + 0xC, 0); }
        } else e.wr8(d + 0xC, 0);
    }
    if (!e.rd8(d + 0xD) && e.rd8(d + 0x11)) {
        const uint32_t g = e.rd32(node + 4), key = e.rd32(d + 0x38);
        uint32_t u = e.rd32(g + 0x1520 + (key & 0x7F) * 4);
        while (u && e.rd32(u + 0xC) != key) u = e.rd32(u + 0xE4);
        if (u) {
            char buf[16];
            const uint32_t sb = e.stk(buf);
            e.h(0xA544, 0, 0, 0, 0, 0, u, sb);
            const uint32_t id = e.h(0xA478, 0, 0, 0, 0, 0, u);
            e.h(0xC8FD0, 0, 0, 0, 0, sb, node, id);
        }
    }
    if ((uint16_t)e.rd16s(node + 0x9C)) {
        const uint32_t pl = e.rd32(ev + 0xC);
        e.h(0x33830, pl ? e.rd32(pl + 0xC) : 0xFFFFFFFFu, node + 0x1C);
    }
    const uint32_t pl = e.rd32(ev + 0xC), game = e.rd32(ev);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, pl);
    const uint32_t fl = e.rd32(pd + e.rd8(game + 0x6D) * 4 + 0x10);
    eax = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xC, 0);
    if (eax == 1) return eax;
    eax = e.h(0xA568, 0, 0, 0, 0, 0, fl, 0xC, 1);
    if (eax == 1) return eax;
    if (e.rd8(node + 0xC) != 3) return eax;
    e.h(0x35EB0, 4, 0, 0, 0, node);
    eax = e.h(0xDFCC0, 0, 0, 0, 0xB2000000u | 0xC3550, 0, 0); e.edi(0xDFCC0, e.rd32(node + 4));
    return eax;
}
}
