// 1.13c Fog+0xD360: uint __stdcall(srv, p2, p3, p4), ret 0x10 -- queue one received packet. The callback at srv+0xBC0 (ECX = p2, EDX = p3, six stack words:
// &d, &off, &idx, &c, 0, 0; all four locals start at 0) returns 1 or 2 to continue, anything else returns 0. The packet buffer comes from the free list
// srv+0x878 (next +0x5BC) under the +0x854 lock (helper 0x11690, EDX 0x9FD), or from 0x1B560 (EAX 0x5C0, ECX 0, file 0x6FF78FAC, line 0xA03 / 0xA0C when the
// list is empty / absent), a null result being a fatal 0x10F30 log in retail (it does not return). Then copy p3 - off bytes from p2 + off to pkt+4,
// pkt[0x5B4] = length, pkt[0x5B8] = GetTickCount, pkt[0x5BC] = 0, pkt[0] = p4. idx < srv[0x87C] (signed) links the packet at the tail of slot idx of the
// +0x870 / +0x874 arrays under the lock (EDX 0xA1F). Returns 1.
#include "fog_env.h"
extern "C" uint32_t fog_d360(const FogEnv* e, uint32_t srv, uint32_t p2, uint32_t p3, uint32_t p4) {
    uint32_t d = 0, off = 0, idx = 0, c = 0;
    uint32_t cb = fe_r32(srv + 0xbc0);
    uint32_t r = fe_call(e, cb & 0xFFFFFF, 0, p2, p3, 0, 0, (uint32_t)(uintptr_t)&d, (uint32_t)(uintptr_t)&off, (uint32_t)(uintptr_t)&idx, (uint32_t)(uintptr_t)&c, 0, 0);
    if (r != 2 && r != 1) return 0;
    uint32_t pkt;
    auto alloc = [&](uint32_t line) {
        uint32_t p = fe_call(e, 0x1B560, 0x5c0, 0, 0, 0, 0, 0x6FF78FAC, line);
        if (!p) fe_call(e, 0x10F30, 0, 0, 0, 0, 0, 2, 0x6FF78FAC, line, 0x6FF768EC);
        return p;
    };
    if (fe_r32(srv + 0x878) == 0) {
        pkt = alloc(0xa0c);
        if (!pkt) return 0;
    } else {
        uint32_t cs = srv + 0x854;
        fe_call(e, 0x11690, 0, cs, 0x9fd);
        fe_enter(e, cs);
        pkt = fe_r32(srv + 0x878);
        if (pkt == 0) { pkt = alloc(0xa03); if (!pkt) return 0; fe_leave(e, cs); }
        else { fe_w32(srv + 0x878, fe_r32(pkt + 0x5bc)); fe_leave(e, cs); }
    }
    uint32_t len = p3 - off;
    memcpy(fe_p(pkt + 4), fe_p(p2 + off), len);
    fe_w32(pkt + 0x5b4, len);
    fe_w32(pkt + 0x5b8, e->now);
    fe_w32(pkt + 0x5bc, 0);
    fe_w32(pkt, p4);
    if ((int32_t)idx < (int32_t)fe_r32(srv + 0x87c)) {
        uint32_t cs = srv + 0x854;
        fe_call(e, 0x11690, 0, cs, 0xa1f);
        fe_enter(e, cs);
        uint32_t tail = fe_r32(fe_r32(srv + 0x874) + idx * 4);
        if (tail == 0) fe_w32(fe_r32(srv + 0x870) + idx * 4, pkt); else fe_w32(tail + 0x5bc, pkt);
        fe_w32(fe_r32(srv + 0x874) + idx * 4, pkt);
        fe_leave(e, cs);
    }
    return 1;
}
