// 1.13c Fog+0xEE60: void __stdcall(srv, client), ret 8 -- drain the receive buffer of one client. srv[0xBC0] == 0 returns. Loop: the client lock
// (client+0x2C, helper 0x11690 with EDX 0x619 / 0x61D around Enter / Leave) brackets the read of client[0x2A]; the callback srv[0xBC0] (ECX = client+0xB0,
// EDX = client[0x2A], six stack words: &used, &skip, &slot, &key, client[0], client[8]) returns 3 (stop), 4 (log PACKET_INVALID through 0xCDA6 / 0x190A0
// and 0xE910 with line 0x630, stop), 5 (only compact) or anything else (take the packet; result 2 first checks the spam list 0xD520(srv, key) when
// srv[0xBD0] is set and bans the sender with line 0x644 on a 0 answer). A packet comes from the free list srv+0x878 (next +0x5BC, lock srv+0x854, EDX 0x659)
// or 0x1B560 (EAX 0x5C0, ECX 0, file 0x6FF78FAC, line 0x65F / 0x668), is filled with used - skip bytes from client+0xB0+skip (client lock EDX 0x675 / 0x678;
// a length below 1 stores byte 0 only), len, GetTickCount, 0, client[0], and linked at the tail of slot (srv[0x870] / srv[0x874], lock EDX 0x68B). The buffer
// is then compacted under the lock (EDX 0x6A6 / 0x6AB): client[0x2A] -= used and the rest moves to client+0xB0. Loops while srv[0xBC0] is set.
#include <cstring>
#include "fog_env.h"
extern "C" void fog_ee60(const FogEnv* e, uint32_t srv, uint32_t client) {
    if (fe_r32(srv + 0xbc0) == 0) return;
    uint32_t buf = client + 0xb0, cs = client + 0x2c;
    for (;;) {
        uint32_t used = 0, skip = 0, slot = 0, key = 0;
        fe_call(e, 0x11690, 0, cs, 0x619);
        fe_enter(e, cs);
        uint32_t size = fe_r32(client + 0xa8);
        fe_call(e, 0x11690, 0, cs, 0x61d);
        fe_leave(e, cs);
        uint32_t cb = fe_r32(srv + 0xbc0);
        uint32_t r = fe_call(e, cb & 0xFFFFFF, 0, buf, size, 0, 0, (uint32_t)(uintptr_t)&used, (uint32_t)(uintptr_t)&skip, (uint32_t)(uintptr_t)&slot,
                             (uint32_t)(uintptr_t)&key, fe_r32(client), fe_r32(client + 0x20));
        if (r == 3) return;
        if (r == 4) {
            uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, fe_r32(client + 0x18));
            fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78ED0, s);
            fe_call(e, 0xE910, srv, 0, 0, 0, 0, fe_r32(client), 0x6FF78FAC, 0x630);
            return;
        }
        if (r != 5) {
            if (r == 2 && fe_r32(srv + 0xbd0) != 0 && fe_call(e, 0xD520, 0, 0, 0, 0, 0, srv, key) == 0) {
                uint32_t s = fe_call(e, 0xCDA6, 0, 0, 0, 0, 0, fe_r32(client + 0x18));
                fe_call(e, 0x190A0, 0, 0, 0, 0, 0, 0x6FF77040, 0x6FF78E88, s);
                fe_call(e, 0xE910, srv, 0, 0, 0, 0, fe_r32(client), 0x6FF78FAC, 0x644);
                return;
            }
            uint32_t pkt;
            if (fe_r32(srv + 0x878) == 0) {
                pkt = fe_call(e, 0x1B560, 0x5c0, 0, 0, 0, 0, 0x6FF78FAC, 0x668);
            } else {
                uint32_t pcs = srv + 0x854;
                fe_call(e, 0x11690, 0, pcs, 0x659);
                fe_enter(e, pcs);
                pkt = fe_r32(srv + 0x878);
                if (pkt == 0) pkt = fe_call(e, 0x1B560, 0x5c0, 0, 0, 0, 0, 0x6FF78FAC, 0x65f);
                else fe_w32(srv + 0x878, fe_r32(pkt + 0x5bc));
                fe_leave(e, pcs);
            }
            int32_t len = (int32_t)(used - skip);
            if (len < 1) {
                *fe_p(pkt) = 0;
            } else {
                fe_call(e, 0x11690, 0, cs, 0x675);
                fe_enter(e, cs);
                memcpy(fe_p(pkt + 4), fe_p(buf + skip), (uint32_t)len);
                fe_call(e, 0x11690, 0, cs, 0x678);
                fe_leave(e, cs);
            }
            fe_w32(pkt + 0x5b4, (uint32_t)len);
            fe_w32(pkt + 0x5b8, e->now);
            fe_w32(pkt + 0x5bc, 0);
            fe_w32(pkt, fe_r32(client));
            uint32_t pcs = srv + 0x854;
            fe_call(e, 0x11690, 0, pcs, 0x68b);
            fe_enter(e, pcs);
            uint32_t tail = fe_r32(fe_r32(srv + 0x874) + slot * 4);
            if (tail == 0) fe_w32(fe_r32(srv + 0x870) + slot * 4, pkt);
            else fe_w32(tail + 0x5bc, pkt);
            fe_w32(fe_r32(srv + 0x874) + slot * 4, pkt);
            fe_leave(e, pcs);
        }
        fe_call(e, 0x11690, 0, cs, 0x6a6);
        fe_enter(e, cs);
        uint32_t nsize = fe_r32(client + 0xa8) - used;
        fe_w32(client + 0xa8, nsize);
        memmove(fe_p(buf), fe_p(buf + used), nsize);
        fe_call(e, 0x11690, 0, cs, 0x6ab);
        fe_leave(e, cs);
        if (fe_r32(srv + 0xbc0) == 0) return;
    }
}
