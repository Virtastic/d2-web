// 1.13c D2Client+0x6A860, written from the disassembly. EAX = unit, plain ret, returns 0 or 1.
// 0xC65E(unit) and 0xC658(unit, &kind, &class, &x, "D2Client.cpp", 0x9B6) give kind, class and x; flag = 0xD07E(). A kind-2 unit looks its class up
// with 0xC226(class) (null: fatal 0x9BE): byte +0x150 and byte [+x+0x118] must be nonzero, else 0; then ebx0 = +0x148 and L18 = +0x14C.
// ebp = 0xC1D2(unit) + ebx0 - (G9960 - Gcc418); ebx = 0xC1F6(unit) + L18 - (G995c - 8). With flag set, Gcc414 == 1 subtracts and == 2 adds
// (G8bc48 / 4, rounded toward zero) to ebp. 0x6A3D0(EAX=unit; ebp, ebx, 0) zero ends with 0. Then 0x68CF0(), 0xC610(unit) (byte), 0xC5EC(unit,
// &o1, &o2); a 0x48-byte frame {+0 = o1, byte +5 = 1, +0x34 = 0, +0x40 = the byte} goes to 0x6E2C0(EAX=-1; unit, &frame, &L18); success runs
// 0xD126(&frame, ebp, ebx, flag with its low byte replaced by the 0x68CF0 byte) and returns 1. gA = {G995c, G9960}, gB = {Gcc414, Gcc418}, gC = {G8bc48} stand for the module globals.
#include <cstring>
#include "cl_env.h"
extern "C" uint32_t d2c_6a860(uint32_t unit, uint32_t gA, uint32_t gB, uint32_t gC, const cl::Env& e) {
    uint32_t kind = e.R(unit), cls = e.R(unit + 4), x = 0;
    x = e.H(0xc65e, unit);
    e.Xs(0x9b6);
    e.H(0xc658, unit, (uint32_t)(uintptr_t)&kind, (uint32_t)(uintptr_t)&cls, (uint32_t)(uintptr_t)&x, 0xB20D4FA4u);
    const uint32_t flag = e.H(0xd07e);
    uint32_t ebx = 0, l18 = 0;
    if (kind == 2) {
        const uint32_t y = e.H(0xc226, cls);
        if (!y) {
            const uint32_t m = e.H(0xbf92, 0x9be);
            e.H(0xbf80, 0xB20D359Fu, m);
            e.H(0x3664, 0xffffffffu);
            return 0;
        }
        if (!(e.R(y + 0x150) & 0xff)) return 0;
        if (!(e.R(y + x + 0x118) & 0xff)) return 0;
        l18 = e.R(y + 0x14c);
        ebx = e.R(y + 0x148);
    }
    const uint32_t g995c = e.R(gA), g9960 = e.R(gA + 4), gc414 = e.R(gB), gc418 = e.R(gB + 4), g8bc48 = e.R(gC);
    ebx -= g9960 - gc418;
    uint32_t ebp = e.H(0xc1d2, unit) + ebx;
    const uint32_t edi = g995c - 8;
    ebx = e.H(0xc1f6, unit) + (l18 - edi);
    if (flag) {
        const uint32_t t = (uint32_t)((int32_t)g8bc48 / 4);
        if (gc414 == 1) ebp -= t;
        else if (gc414 == 2) ebp += t;
    }
    if (!e.H(0x6a3d0, ebp, ebx, 0, 0, 0, unit)) return 0;
    const uint32_t lo = e.H(0x68cf0) & 0xff;   // the byte lands in the low byte of the flag slot, the 4th 0xD126 argument
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
    const uint32_t ok = e.H(0x6e2c0, unit, fp, (uint32_t)(uintptr_t)&l18, 0, 0, 0xffffffffu);
    if (e.blk) e.blk(e.ctx, 0x6e2c0, sp, 18);
    if (!ok) return 0;
    memcpy(snap, fr, sizeof fr);
    e.H(0xd126, fp, ebp, ebx, (flag & 0xffffff00u) | lo);
    snap[1] = 0;
    if (e.blk) e.blk(e.ctx, 0xd126, sp, 18);
    return 1;
}
