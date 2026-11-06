// 1.13c D2Client+0x6AA30, written from the disassembly. ESI = unit, plain ret, returns 0 or 1.
// A unit with bit 5 of +0xC4 set, or one where 0xC1FC(unit, 0x92) is nonzero, returns 0. 0xC65E(unit) presets x; 0xC658(unit, &kind, &class, &x,
// "D2Client.cpp", 0x86F) gives kind, class and x; flag = 0xD07E(). A kind-2 unit looks its class up with 0xC226(class) (null: fatal 0x87A): byte +0x150 and
// byte [+x+0x118] must be nonzero, else 0; then ebp0 = +0x148 and L = +0x14C. p4 = ([[unit+0x54]+0x30]+4) >> 11 and p8 = (... +8) >> 11 (0 on a null link).
// ebp = 0xC1D2(unit) + ebp0 + p4 - (G9960 - Gcc418); ebx = 0xC1F6(unit) + L + p8 - (G995c - 8). With the flag set ebx += 0x10 and Gcc414 == 1 subtracts,
// == 2 adds (G8bc48 / 4, toward zero) to ebp. 0x6A3D0(EAX=unit; ebp, ebx, 1) zero ends with 0. Then 0xC610(unit) (byte), 0xC5EC(unit, &o1, &o2); a 0x48-byte
// frame {+0 = o1, byte +5 = 1, +0x34 = 0, +0x40 = the byte} goes to 0x6E2C0(EAX=-1; unit, &frame, &flag slot); success runs 0xD12C(&frame, ebp, ebx) and returns 1.
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6aa30(uint32_t unit, uint32_t gA, uint32_t gB, uint32_t gC, const cl::Env& e) {
    if (e.R(unit + 0xc4) & 0x20) return 0;
    if (e.H(0xc1fc, unit, 0x92)) return 0;
    uint32_t kind = e.R(unit), cls = e.R(unit + 4), x = 0;
    x = e.H(0xc65e, unit);
    e.Xs(0x86f);
    e.H(0xc658, unit, (uint32_t)(uintptr_t)&kind, (uint32_t)(uintptr_t)&cls, (uint32_t)(uintptr_t)&x, 0xB20D4FA4u);
    const uint32_t flag = e.H(0xd07e);
    uint32_t ebp = 0, l = 0;
    if (kind == 2) {
        const uint32_t y = e.H(0xc226, cls);
        if (!y) {
            const uint32_t m = e.H(0xbf92, 0x87a);
            e.H(0xbf80, 0xB20D359Fu, m);
            e.H(0x3664, 0xffffffffu);
            return 0;
        }
        if (!(e.R(y + 0x150) & 0xff)) return 0;
        if (!(e.R(y + x + 0x118) & 0xff)) return 0;
        ebp = e.R(y + 0x148);
        l = e.R(y + 0x14c);
    }
    const uint32_t g995c = e.R(gA), g9960 = e.R(gA + 4), gc414 = e.R(gB), gc418 = e.R(gB + 4), g8bc48 = e.R(gC);
    uint32_t p4 = 0, p8 = 0;
    const uint32_t a = e.R(unit + 0x54);
    if (a) {
        const uint32_t b = e.R(a + 0x30);
        if (b) { p4 = (uint32_t)((int32_t)e.R(b + 4) >> 11); p8 = (uint32_t)((int32_t)e.R(b + 8) >> 11); }
    }
    ebp = e.H(0xc1d2, unit) + ebp + (p4 - (g9960 - gc418));
    uint32_t ebx = e.H(0xc1f6, unit) + l + (p8 - (g995c - 8));
    if (flag) {
        const uint32_t t = (uint32_t)((int32_t)g8bc48 / 4);
        if (gc414 == 1) ebp -= t;
        else if (gc414 == 2) ebp += t;
        ebx += 0x10;
    }
    if (!e.H(0x6a3d0, ebp, ebx, 1, 0, 0, unit)) return 0;
    const uint32_t cb = e.H(0xc610, unit) & 0xff;
    uint32_t o[2] = { 0, 0 };
    e.H(0xc5ec, unit, (uint32_t)(uintptr_t)&o[0], (uint32_t)(uintptr_t)&o[1]);
    uint32_t fr[18] = {}, snap[18];
    fr[0] = o[0];
    ((uint8_t*)fr)[5] = 1;
    fr[0x34 / 4] = 0;
    fr[0x40 / 4] = cb;
    const uint32_t fp = (uint32_t)(uintptr_t)fr, sp = (uint32_t)(uintptr_t)snap;
    memcpy(snap, fr, sizeof fr);
    const uint32_t ok = e.H(0x6e2c0, unit, fp, (uint32_t)(uintptr_t)&l, 0, 0, 0xffffffffu);
    if (e.blk) e.blk(e.ctx, 0x6e2c0, sp, 18);
    if (!ok) return 0;
    memcpy(snap, fr, sizeof fr);
    e.H(0xd12c, fp, ebp, ebx);
    snap[1] = 0;   // the capture masks frame word 1 (hookblk mask 0x3fffd)
    if (e.blk) e.blk(e.ctx, 0xd12c, sp, 18);
    return 1;
}
