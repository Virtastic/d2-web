// D2Client+0x854E0 -- item packet that confirms an item on a unit's owner (equip / belt move).
#include "d2client_854e0.h"

// 1.13c D2Client+0x854E0
void d2c_854e0(const D2Client854e0Env& e, U pkt) {
    uint8_t hdr[24];
    e.f11105(pkt + 0xd, e.rd(pkt, 2, 1) - 0xd, 0, hdr);
    auto h8 = [&](uint32_t o) { return (uint32_t)hdr[o]; };
    auto h16 = [&](uint32_t o) { return (uint32_t)hdr[o] | (uint32_t)hdr[o + 1] << 8; };
    auto h32 = [&](uint32_t o) { return (uint32_t)hdr[o] | (uint32_t)hdr[o + 1] << 8 | (uint32_t)hdr[o + 2] << 16 | (uint32_t)hdr[o + 3] << 24; };
    const uint32_t pid = e.rd(pkt, 4, 4);
    const U mon = e.fa4e20(e.rd(pkt, 9, 4), e.rd(pkt, 8, 1));
    if (!mon) return;
    const uint32_t owner = e.rd(mon, 0x60, 4);
    if (!owner) e.die(0xb77);
    U item = 0, ebx = mon;
    if ((uint32_t)mon == e.self) {
        U it = (U)e.rd(e.itTbl, (pid & 0x7f) * (uint32_t)sizeof(U), 4);
        while (it && e.rd(it, 0xc, 4) != pid) it = (U)e.rd(it, 0xe4, 4);
        if (it && e.rd(it, 0, 4) != 4) { e.die(0x5a); goto fatal; }
        item = it;
        if (!item) return;
        if ((U)e.f10646(owner, item) != item) { e.die(0xb83); goto fatal; }
        const uint32_t q = e.f10584(mon, 0, e.g9854);
        e.f10869(0xb85, owner, h16(4), h16(6), q, 1, 0);
        e.f8bbf0();
        e.f6e2b0();
    }
    if (e.f10197(h32(0xc), 1)) {
        e.f56ef0(e.itTbl, pid & 0x7f, pid, 4);
        item = e.f833f0(pkt, hdr, 0, 0);
        if (!item) { e.die(0xb8d); ebx = 0; goto fatal; }
        goto after;
    }
    ebx = 0;
    if (item) goto fatal;
    item = e.f833f0(pkt, hdr, 0, 0);
    if (!item) { e.die(0xb93); goto fatal; }
    goto after;
fatal:
    e.f82980(item, 0, h8(8), 0, h32(0x10), (uint32_t)ebx);
after:
    e.f10058(item, h32(0x11));
    if (!e.f10405(owner, item, (h8(0x11) == 0xb || h8(0x11) == 0xc) ? 4 : 3)) return;
    if (!e.f81b40(item, h8(0x11), owner)) return;
    e.f693c0(mon);
    e.f10138(owner, item);
    e.f6a220(mon);
    e.f81ff0(mon, item, mon);
    e.f82fe0(mon);
    e.f82fe0(mon);
    e.f81aa0();
}
