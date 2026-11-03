// 1.13c D2Client+0x4F4A0 -- per-unit client update (EAX = unit, caller 0x2B1E0). Player units only (type 1), mode below 0x10.
// Order: 0xC1FC(unit, 1) then (if non-zero) 0xA5440(EAX = unit) must pass; state = 0x4BFF0() (EDI = unit; null returns); 0x4F2C0(EAX = unit);
// with state[0] == 1, local = 0x2A700(EAX = unit). Mode 1 and 0x1E40(EAX = unit, 1) == 0: with a unit object (+0x2C) that 0xC2DA accepts,
// 0xCD54(obj, &a, &b) fills a pair, 0xA5B40(EAX = its result, ECX = b, EDX = a) gives r, 0xC364(obj, r); r == 0 skips 0xCDC6(unit).
// Then rec = 0x1230(EAX = class) (null returns); when bitmask byte +4 & rec[0xD], 0x1510(EAX = class, EDX = 0xD) == 0, mode == 1, bit 9 of +0xC4
// clear, class not 0x219/0x21A/0x21B, player slot (0x11BBFC) set, 0xCC70(unit, player) < 7 and 0x1330(EAX = 0x64, ECX = unit + 0x20) < 10:
// 0xC478(obj, 0x1630(ECX = player), 0x1660(ECX = player), 0). Next r = 0x4EE40(state); state[2] picks the flag: 1 local, 2 and 3 r,
// 4 local and 0xC46C(unit), else 0. 0x4F0B0(EAX = unit), 0x212F0(unit, 0, 2), 0xCD84(obj) if any, 0x4DA60(EAX = unit) on the flag.
// Class 0x210 in mode 0 with bit 0 of +0xC4 set clears it and calls 0xA7BF0(ECX = 0x1DE, EDX = 0x1630(unit), 0x1660(unit), 2, 0).
// Last: 0xC78A(unit) non-zero runs 0xA08E0(unit); then 0x237E0(unit), 0x23C00(unit).
#include "cl_env.h"
extern "C" void d2c_4f4a0(uint32_t u, const cl::Env& e) {
    if (!u || e.R(u) != 1) return;
    if (e.H(0xc1fc, u, 1) && !e.H(0xa5440, 0, 0, 0, 0, 0, u)) return;
    int32_t mode = (int32_t)e.R(u + 0x10);
    if (mode >= 0x10) return;
    uint32_t st = e.H(0x4bff0);
    if (!st) return;
    e.H(0x4f2c0, 0, 0, 0, 0, 0, u);
    uint32_t local = 0;
    if (e.R(st) == 1) local = e.H(0x2a700, 0, 0, 0, 0, 0, u);
    if (mode == 1 && e.H(0x1e40, 1, 0, 0, 0, 0, u) == 0) {
        uint32_t obj = e.R(u + 0x2c);
        bool run = true;
        if (obj && e.H(0xc2da, obj)) {
            uint32_t o[2] = {};
            uint32_t op = (uint32_t)(uintptr_t)o;
            uint32_t r = e.H(0xcd54, obj, op, op + 4);
            uint32_t b = e.H(0xa5b40, 0, 0, 0, 0, 0, r, o[1], o[0]);
            e.H(0xc364, obj, b);
            if (!b) run = false;
        }
        if (run) e.H(0xcdc6, u);
    }
    uint32_t cls = e.R(u + 4);
    uint32_t rec = e.H(0x1230, 0, 0, 0, 0, 0, cls);
    if (!rec) return;
    if ((e.R8(e.bm + 4) & e.R8(rec + 0xd)) && e.H(0x1510, 0, 0, 0, 0, 0, cls, 0, 0xd) == 0 && e.R(u + 0x10) == 1 &&
        !((e.R(u + 0xc4) >> 9) & 1) && cls != 0x219 && cls != 0x21a && cls != 0x21b && e.self) {
        if (e.H(0xcc70, u, e.self) < 7 && (int32_t)e.H(0x1330, 0, 0, 0, 0, 0, 0x64, u + 0x20) < 10) {
            uint32_t obj = e.R(u + 0x2c);
            if (obj) {
                uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, e.self);
                uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, e.self);
                e.H(0xc478, obj, x, y, 0);
            }
        }
    }
    uint32_t r = e.H(0x4ee40, st);
    uint32_t flag = 0;
    switch (e.R(st + 8)) {
        case 1: flag = local; break;
        case 2: case 3: flag = r; break;
        case 4: if (local && e.H(0xc46c, u)) flag = 1; break;
    }
    e.H(0x4f0b0, 0, 0, 0, 0, 0, u);
    e.H(0x212f0, u, 0, 2);
    if (uint32_t obj = e.R(u + 0x2c)) e.H(0xcd84, obj);
    if (flag) e.H(0x4da60, 0, 0, 0, 0, 0, u);
    if (e.R(u + 0x10) == 0 && e.R(u + 4) == 0x210) {
        uint32_t f = e.R(u + 0xc4);
        if (f & 1) {
            e.W(u + 0xc4, f & ~1u);
            uint32_t y = e.H(0x1660, 0, 0, 0, 0, 0, 0, u);
            uint32_t x = e.H(0x1630, 0, 0, 0, 0, 0, 0, u);
            e.H(0xa7bf0, y, 2, 0, 0, 0, 0, 0x1de, x);
        }
    }
    if (e.H(0xc78a, u)) e.H(0xa08e0, u);
    e.H(0x237e0, u);
    e.H(0x23c00, u);
}
