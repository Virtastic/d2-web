// 1.13c D2Client+0x2BF40 (ECX = unit, EDX = skill id, stack word 0 = sa, ret 4): starts a skill cast animation on a unit.
// Ported from the x86 (the decompile loses the register and spill-slot arguments); callees reach through the injected dispatcher.
#include <cstdint>
#include <cstring>
#include "d2client_callee.h"
static inline uint32_t rd32_2bf40(uint32_t a) { uint32_t x; std::memcpy(&x, (const void*)(uintptr_t)a, 4); return x; }
static inline int16_t rd16_2bf40(uint32_t a) { int16_t x; std::memcpy(&x, (const void*)(uintptr_t)a, 2); return x; }
// 1.13c D2Client+0x2BF40
// cbbfc = [0x11BBFC] (local unit); ptab = *[[0xCE474]] (the D2Common data tables: skill rows at +0xB98, count at +0xBA0, stride 0x23C).
extern "C" uint32_t d2c_2bf40(const D2cSvc& v, uint32_t unit, int32_t skill, uint32_t sa, uint32_t cbbfc, uint32_t ptab) {
    const uint32_t STR = 0xB2000000u | 0xD6A48u;                                   // "..\Source\D2Client\SKILLS\SkillsPal.cpp"
    const uint32_t p = d2c_call(v, 0xc310, 0, 0, 0, 0, 0, unit);                   // #10511(unit)
    if (!p) return 0;
    if (skill < 0 || skill >= (int32_t)rd32_2bf40(ptab + 0xba0)) return 0;
    const uint32_t rec = (uint32_t)skill * 0x23c + rd32_2bf40(ptab + 0xb98);
    if (!rec) return 0;
    if ((int32_t)d2c_call(v, 0xc42a, 0, 0, 0, 0, 0, p, STR, 0x176) == skill && unit == cbbfc &&
        (d2c_call(v, 0xc628, 0, 0, 0, 0, 0, p) & 1)) return 0;
    const uint32_t u2c = rd32_2bf40(unit + 0x2c);
    if (!u2c) return 0;
    const uint32_t cur = d2c_call(v, 0xa5f70, 0, 0, 0, 0, 0);
    const uint32_t t = rd32_2bf40(unit);
    if (t == 0 && cur && d2c_call(v, 0xc6e2, 0, 0, 0, 0, 0, unit, cur, 0))
        return d2c_call(v, 0xa3040, cur, 0, 0, 0, 0, unit);
    bool go = false;
    if (t == 0) go = rd16_2bf40(rec + 0x102) > 0;
    else if (t == 1) go = (int32_t)d2c_call(v, 0xc79c0, unit, 0, 0, 0, 0) > 0;
    if (go) d2c_call(v, 0x88a70, 0, 0, 0, 0, 0, unit, 0, 0, 0);
    uint32_t sub = 0;
    if (!cur || d2c_call(v, 0xc1fc, 0, 0, 0, 0, 0, cur, 0x36)) d2c_call(v, 0xcb50, 0, 0, 0, 0, 0, p, 6);
    else { d2c_call(v, 0xcb50, 0, 0, 0, 0, 0, p, rd32_2bf40(cur)); sub = rd32_2bf40(cur + 0xc); }
    d2c_call(v, 0xcb7a, 0, 0, 0, 0, 0, p, sub);
    d2c_call(v, 0xc74e, 0, 0, 0, 0, 0, p, 7);
    int32_t speed = 0x100;
    if (t == 0) {
        const uint32_t q = d2c_call(v, 0x1300, rd32_2bf40(unit + 4), 0, 0, 0, 0);
        if (q) speed = (int32_t)((uint32_t)(*(const uint8_t*)(uintptr_t)(q + 0x41)) << 8);
    } else {
        const uint32_t id = rd32_2bf40(unit + 4), q = d2c_call(v, 0x1230, id, 0, 0, 0, 0);
        if (q) {
            speed = (int32_t)rd16_2bf40(q + 0x34) * 256;
            if (id == 0xd3) d2c_call(v, 0xc74e, 0, 0, 0, 0, 0, p, 0xf);
            d2c_call(v, 0xc3b8, 0, 0, 0, 0, 0, u2c, 0x1e);
        }
    }
    int32_t bn = (int32_t)d2c_call(v, 0xc1c6, 0, 0, 0, 0, 0, unit, 0x43, 0);
    if (bn <= 0x32) bn = 0x32;
    const uint32_t x = d2c_call(v, 0x2d20, (uint32_t)skill, 0, 0, 0, 0, STR, 0x1ca) + (uint32_t)bn;
    const uint32_t a = d2c_call(v, 0x14a0, x, 100, (uint32_t)speed, 0, 0);
    d2c_call(v, 0xc328, 0, 0, 0, 0, 0, sa, a);
    d2c_call(v, 0xc376, 0, 0, 0, 0, 0, sa, unit, 0);
    if (cur) d2c_call(v, 0xa5fb0, unit, 0, 0, 0, 0, 1);
    d2c_call(v, 0xc60a, 0, 0, 0, 0, 0, p, 0x1001);
    d2c_call(v, 0xcb8c, 0, 0, 0, 0, 0, p, 0);
    d2c_call(v, 0xa3220, 0, 0, 0, 0, 0);
    return 1;
}
