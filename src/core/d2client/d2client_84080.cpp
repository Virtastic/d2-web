// D2Client+0x84080 -- item packet move/remove on a monster unit's owner.
#include "d2client_84080.h"

static U chain(const D2Client84080Env& e, U head, uint32_t id, bool isMon) {
    // walks +0xE4 until +0xC == id; returns 0 on a miss
    U u = (U)e.rd(head, 0, 4); (void)isMon;
    while (u && e.rd(u, 0xC, 4) != id) u = (U)e.rd(u, 0xE4, 4);
    return u;
}

// 1.13c D2Client+0x84080
void d2c_84080(const D2Client84080Env& e, U pkt) {
    uint8_t hdr[24];
    e.f11105(pkt + 8, e.rd(pkt, 2, 1) - 8, 0, hdr);
    auto h8 = [&](uint32_t o) { return (uint32_t)hdr[o]; };
    auto h16 = [&](uint32_t o) { return (uint32_t)hdr[o] | (uint32_t)hdr[o + 1] << 8; };
    auto h32 = [&](uint32_t o) { return (uint32_t)hdr[o] | (uint32_t)hdr[o + 1] << 8 | (uint32_t)hdr[o + 2] << 16 | (uint32_t)hdr[o + 3] << 24; };
    if (h8(0x10) == 0xff || !e.flag) return;
    const uint32_t id = e.id;
    U mon = (U)e.rd(e.monTbl, (id & 0x7f) * (uint32_t)sizeof(U), 4);
    if (!mon) return;
    while (e.rd(mon, 0xC, 4) != id) { mon = (U)e.rd(mon, 0xE4, 4); if (!mon) return; }
    if (e.rd(mon, 0, 4) != 1) e.die();
    const uint32_t owner = e.rd(mon, 0x60, 4);
    if (!owner) return;
    const uint32_t pid = e.rd(pkt, 4, 4);
    U item = (U)e.rd(e.itTbl, (pid & 0x7f) * (uint32_t)sizeof(U), 4);
    while (item && e.rd(item, 0xC, 4) != pid) item = (U)e.rd(item, 0xE4, 4);
    if (item && e.rd(item, 0, 4) != 4) item = (U)e.die();
    if (e.rd(pkt, 1, 1) == 0xC) {
        if (!item) return;
        if (e.g9663) e.f8bbf0();
        if (!e.f10646(owner, item)) return;
        const uint32_t c = e.f10020(item) & 0xFF;
        if (c == 0xFF) return;
        const uint32_t cs = (owner & ~0xFFu) | c;     // retail reloads the dword of the stack slot whose low byte holds c
        const uint32_t q = e.f10584(mon, cs, e.g9854);
        e.f10869(0x81b, owner, h16(4), h16(6), q, 1, cs);
        e.f6e2b0();
        e.f56ef0(e.itTbl, pid & 0x7f, pid, 4);
        e.f47a70(pid);
        return;
    }
    if (!item) {
        item = e.f835c0(pkt, hdr, 0, 0);
        e.f10012(item, h32(0x10));
    } else {
        e.f10391(owner, item);
        if (!e.f10197(h32(0xC), 1)) e.f82980(item, 0, 0, 0, h32(0x10));
        else {
            e.f56ef0(e.itTbl, pid & 0x7f, pid, 4);
            item = e.f835c0(pkt, hdr, 0, 0);
            if (!item) return;
            if ((e.f10020(item) & 0xFF) != h8(0x10)) return;
        }
    }
    e.f10012(item, h32(0x10));
    uint32_t o1 = 0, o2 = 0;
    const uint32_t q = e.f10584(mon, h32(0x10), e.g9854);
    if (!e.f11148(owner, item, h16(4), h16(6), q, &o1, &o2, h32(0x10))) return;
    e.f82660(item, h32(4), h32(6), h32(0x10), mon);
    *e.bc54 = 1;
    e.f81aa0();
}
