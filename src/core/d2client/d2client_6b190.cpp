// 1.13c D2Client+0x6B190, written from the disassembly. stdcall 4: unit, id, dx, dy.
// Walks the node chain at [[unit+0x54]+0x2c] (next at +0xA4). A node counts when its flag (+0x34 == 0 on pass 0; the second
// pass never runs a node because ESI is spent) is set, kind 6 has +8 and kind 8 has +0x3c, and +4 == id. Then the Data row
// (*sgptDataTables +0xBBC, count +0xBC0, stride 0x84) must exist; a 0x48-byte frame {+0 = node+0x18>>8, +5 = 1, +0x14 = id,
// +0x40 = D2Common #10663(unit) & 0xFF} is filled by 0x6E2C0(EAX=-1; 0, &frame, &local); on success, if node+0x18>>8 is
// below node+0x1c>>8, 0xD096(&frame, node+0x20+dx, node+0x24+dy, -128, row[0x7C], 0) draws it.
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_6b190(uint32_t unit, uint32_t id, uint32_t dx, uint32_t dy, const cl::Env& e) {
    if (!unit) return;
    const uint32_t pa = e.R(unit + 0x54);
    if (!pa) return;
    uint32_t n = e.R(pa + 0x2c);
    if (!n) return;
    for (uint32_t pass = 0; pass < 2; ++pass) {
        if (!n) continue;
        do {
            uint32_t f = 1;
            const uint32_t n34 = e.R(n + 0x34);
            if (n34 && pass == 0) f = 0;
            if (!n34 && pass != 0) f = 0;
            const uint32_t kind = e.R(n);
            if (kind == 6 && !e.R(n + 8)) f = 0;
            if (kind == 8 && !e.R(n + 0x3c)) f = 0;
            if (e.R(n + 4) == id && f) {
                const int32_t hi = (int32_t)e.R(n + 0x18) >> 8;
                if ((int32_t)id < 0) return;
                if ((int32_t)id >= (int32_t)e.R(e.dt + 0xbc0)) return;
                const uint32_t row = e.R(e.dt + 0xbbc) + id * 0x84u;
                if (!row) return;
                uint32_t fr[18] = {}, loc = 0, snap[18];
                fr[5 / 4] = 0;
                fr[0x14 / 4] = id;
                const uint32_t fp = (uint32_t)(uintptr_t)fr, sp = (uint32_t)(uintptr_t)snap;
                fr[0x40 / 4] = e.H(0xc610, unit) & 0xff;
                fr[0x34 / 4] = 0;
                fr[0] = (uint32_t)hi;
                ((uint8_t*)fr)[5] = 1;
                memcpy(snap, fr, sizeof fr);
                const uint32_t ok = e.H(0x6e2c0, 0, fp, (uint32_t)(uintptr_t)&loc, 0, 0, 0xffffffffu);
                if (e.blk) e.blk(e.ctx, 0x6e2c0, sp, 18);
                if (!ok) return;
                if (hi < ((int32_t)e.R(n + 0x1c) >> 8)) {
                    memcpy(snap, fr, sizeof fr);
                    e.Xs(0, 0);
                    e.H(0xd096, fp, e.R(n + 0x20) + dx, e.R(n + 0x24) + dy, 0xffffff80u, e.R8(row + 0x7c));
                    if (e.blk) e.blk(e.ctx, 0xd096, sp, 18);
                }
            }
            n = e.R(n + 0xa4);
        } while (n);
    }
}
