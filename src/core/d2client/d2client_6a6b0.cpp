// 1.13c D2Client+0x6A6B0, written from the disassembly. EAX = unit; stack p4 (a byte, passed on BY ADDRESS
// to 0x6E2C0), p8 and p12 (out pointers), ret 0xC.
// D2Common #10... 0xC65E(unit) and 0xC658(unit, &kind, &class, &x, "D2Client.cpp", 0xD4D) give the unit's kind, class and x; w = unit[+0x60] or, with the
// unit's flag set (+0xC8 bit 3) and 0xCBDA(unit, 0x25) and 0xA60D0(unit) nonzero, that object's +0x60; id = 0xC2AA(unit, w, &out, x, 0).
// pData = unit[+0x54] (null: fatal line 0x3AC). Look {id1 = 0x1AF0(), id} up in pData[+0x28] (nodes {a, b, rec, ..., next +0x10}); a miss runs
// 0x6A2F0(EAX=-1, ECX=id, EDX=unit) and, when it fails, ends. Then the node with a == x; its rec[+0x14] must be nonzero. A 0x48-byte frame
// {+0 = unit[+0x44]>>8, byte +5 = p4, +0x34 = 0, +0x40 = 0xC2D4(path) & 0xFF} is filled by 0x6E2C0(EAX=-1; unit, &frame, &p4), opened by
// 0xC0D0(&frame, 0, 1); cel = frame[+0x3C] (null: fatal 0xD7A); *p8 = 0xC0CA(cel), *p12 = 0xC0DC(cel).
#include <cstring>
#include "cl_env.h"
extern "C" void d2c_6a6b0(uint32_t unit, uint32_t p4, uint32_t p8, uint32_t p12, const cl::Env& e) {
    const uint32_t kind0 = e.R(unit), cls0 = e.R(unit + 4), path = e.R(unit + 0x2c);
    uint32_t kind = kind0, cls = cls0, loc2 = 0;
    uint32_t x = e.H(0xc65e, unit);
    e.Xs(0xd4d);
    e.H(0xc658, unit, (uint32_t)(uintptr_t)&kind, (uint32_t)(uintptr_t)&cls, (uint32_t)(uintptr_t)&x, 0xB20D4FA4u);
    uint32_t src = unit;
    if ((e.R(unit + 0xc8) >> 3) & 1) {
        if (e.H(0xcbda, unit, 0x25)) {
            const uint32_t r = e.H(0xa60d0, 0, 0, 0, 0, 0, unit);
            if (r) src = r;
        }
    }
    const uint32_t w = e.R(src + 0x60);
    const uint32_t id = e.H(0xc2aa, unit, w, (uint32_t)(uintptr_t)&loc2, x, 0);
    const uint32_t pd = e.R(unit + 0x54);
    if (!pd) {
        const uint32_t m = e.H(0xbf92, 0x3ac);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    const uint32_t id1 = e.H(0x1af0);
    bool hit = false;
    for (uint32_t n = e.R(pd + 0x28); n; n = e.R(n + 0x10))
        if (e.R(n) == id1 && e.R(n + 4) == id) { hit = true; break; }
    if (!hit && !e.H(0x6a2f0, 0, 0, 0, 0, 0, 0xffffffffu, id, unit)) return;
    const uint32_t pd2 = e.R(unit + 0x54);
    if (!pd2) return;
    uint32_t n = e.R(pd2 + 0x28);
    if (!n) return;
    while (e.R(n) != x) { n = e.R(n + 0x10); if (!n) return; }
    const uint32_t rec = e.R(n + 8);
    if (!e.R(rec + 0x14)) return;
    uint32_t fr[18] = {}, snap[18], p4v = p4;
    const int32_t hi = (int32_t)e.R(unit + 0x44) >> 8;
    fr[0x40 / 4] = e.H(0xc2d4, path) & 0xff;
    const uint8_t cl = (uint8_t)p4v;
    fr[0x34 / 4] = 0;
    fr[0] = (uint32_t)hi;
    ((uint8_t*)fr)[5] = cl;
    const uint32_t fp = (uint32_t)(uintptr_t)fr, sp = (uint32_t)(uintptr_t)snap;
    memcpy(snap, fr, sizeof fr);
    const uint32_t ok = e.H(0x6e2c0, unit, fp, (uint32_t)(uintptr_t)&p4v, 0, 0, 0xffffffffu);
    if (e.blk) e.blk(e.ctx, 0x6e2c0, sp, 18);
    if (!ok) return;
    memcpy(snap, fr, sizeof fr);
    const uint32_t op = e.H(0xc0d0, fp, 0, 1);
    if (e.blk) e.blk(e.ctx, 0xc0d0, sp, 18);
    if (!op) return;
    uint32_t cel = fr[0x3c / 4];
    if (!cel) {
        const uint32_t m = e.H(0xbf92, 0xd7a);
        e.H(0xbf80, 0xB20D359Fu, m);
        e.H(0x3664, 0xffffffffu);
        return;
    }
    e.W(p8, e.H(0xc0ca, cel));
    e.W(p12, e.H(0xc0dc, fr[0x3c / 4]));
}
