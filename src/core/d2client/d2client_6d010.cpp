// D2Client+0x6D010 -- walk every unit of the self unit's list and every node on its chain, handing each to 0x6C3F0 or 0x6C3B0 by kind.
#include "d2client_6d010.h"

// 1.13c D2Client+0x6D010
uint32_t d2c_6d010(const D2Client6d010Env& e, U arg) {
    U edi = arg;
    const uint32_t h = e.fc1f0(e.self);
    if (!h) return h;
    U arr = 0; uint32_t cnt = 0;
    e.fc3dc(h, &arr, &cnt);
    uint32_t ctr = 0;
    for (uint32_t i = 0; i < cnt; i++) {
        if (e.g04c() && (!edi || e.g054())) {
            const uint32_t al = ctr & 0x1f; ctr++;
            if (al) e.fc1760(1);              // retail: test al,al; je skip
        }
        if (!e.g04c()) { const uint32_t v = e.fc11e(); if (v >= 0x1e) return v; }
        U node = e.ldp(e.ldp(arr, i * 4u), 0x74);
        while (node) {
            uint32_t type = e.ld32(node, 0), idx = e.ld32(node, 4), aux = 0;
            aux = e.fc65e(node);
            e.fc658(node, &type, &idx, &aux);
            if (type == 1) {
                if ((int32_t)idx < e.limit() && !(aux != 0xc ? false : e.recWord(idx) != 0))
                    e.f6c3f0(node, edi);
            } else if (type == 0) {
                if (e.g054()) edi = 0;
                e.f6c3b0(node, edi);
            }
            if (!e.g04c()) { const uint32_t v = e.fc11e(); if (v >= 0x1e) return v; }
            node = e.ldp(node, 0xe8);
        }
    }
    return cnt;                               // retail: eax = the count re-read at the loop test
}
