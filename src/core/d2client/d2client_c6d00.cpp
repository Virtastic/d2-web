// 1.13c D2Client+0xC6D00, written from the disassembly. ECX = unit, one unused stack word (ret 4). t = 0xC838(ECX = unit): 0 returns 0. x = 0xC628(t), res = 0.
// x & 0x100: r = 0xC6AA0(EAX = unit; t, 1). r == 0 returns 0 unless x & 2 (then on to the box step below). r != 0: k = 0xCB9E(t), v = 0xC79C(t); when k != 6
// rec = 0xA4E20(EAX = v & 0x7F, ECX = v, EDX = module 0x10A608 + (k << 9); k) is the record; with none (or k == 6) 0xA1860(unit, 0, 0, 3, 0xA783, module 0xA09F0,
// &{0, -1, 0, -1, -1, 0}, 0) supplies X[0], else X[2]; with neither [unit+0x48] = 0x400. A found record o: 0xC62E(unit, o), 0xA5FB0(EAX = unit; 0), 0xC7A8(ECX = unit),
// 0x88A70(EBX = 0x22280(ECX = unit); unit, 0, 0, 0), res = 1. Then x & 2: the unit's box (type 2, or 4..5: [path+0xC] / [path+0x10]; else 0xC3BE / 0xC2EC of the path or 0)
// feeds 0xC6060(ESI = unit, EBX = width; height) and 0xC60A(t, 0), return 1. Else x & 0x80: 0xC5F20(EDI = unit; t) and 0xC65B0(EAX = t; unit), return 1.
// Else 0xC60A(t, 0x1000) and return res.
#include "cl_mb2.h"
static bool isbox(int32_t t) { return t == 2 || (t > 3 && t <= 5); }
// 1.13c D2Client+0xC6D00
extern "C" uint32_t d2c_c6d00(uint32_t u, uint32_t, const clm::Env& e) {
    using clm::Env;
    const uint32_t t = e.H(0xc838, 0, u);
    if (!t) return 0;
    const uint32_t x = e.H(0xc628, 0, 0, 0, 0, 0, t);
    uint32_t res = 0;
    bool box = (x & 2) != 0;
    if (x & 0x100) {
        const uint32_t r = e.H(0xc6aa0, u, 0, 0, 0, 0, t, 1);
        if (!r) {
            if (!(x & 2)) return r;
        } else {
            const uint32_t k = e.H(0xcb9e, 0, 0, 0, 0, 0, t);
            const uint32_t v = e.H(0xc79c, 0, 0, 0, 0, 0, t);
            uint32_t o = 0;
            if (k != 6) o = e.H(0xa4e20, v & 0x7f, v, Env::Mod(0x10a608 + (k << 9)), 0, 0, k);
            if (!o) {
                uint32_t X[6] = { 0, 0xffffffffu, 0, 0xffffffffu, 0xffffffffu, 0 };
                e.H(0xa1860, 0, 0, 0, 0, 0, u, 0, 0, 3, 0xa783, Env::Mod(0xa09f0), (uint32_t)(uintptr_t)X);
                o = X[0];
                if (!o) o = X[2];
            }
            if (!o) {
                e.W(u + 0x48, 0x400);
            } else {
                e.H(0xc62e, 0, 0, 0, 0, 0, u, o);
                e.H(0xa5fb0, u, 0, 0, 0, 0, 0);
                e.H(0xc7a8, 0, u);
                const uint32_t eb = e.H(0x22280, 0, u);
                e.H(0x88a70, 0, 0, 0, eb, 0, u, 0, 0, 0);
                res = 1;
            }
        }
    }
    if (x & 2) {
        const int32_t ty = (int32_t)e.R(u);
        const uint32_t p = e.R(u + 0x2c);
        const uint32_t hgt = isbox(ty) ? e.R(p + 0x10) : (p ? e.H(0xc2ec, 0, 0, 0, 0, 0, p) : 0u);
        const int32_t ty2 = (int32_t)e.R(u);
        const uint32_t p2 = e.R(u + 0x2c);
        const uint32_t wid = isbox(ty2) ? e.R(p2 + 0xc) : (p2 ? e.H(0xc3be, 0, 0, 0, 0, 0, p2) : 0u);
        e.H(0xc6060, 0, 0, 0, wid, u, hgt);
        e.H(0xc60a, 0, 0, 0, 0, 0, t, 0);
        return 1;
    }
    if (x & 0x80) {
        e.H(0xc5f20, 0, 0, 0, 0, 0, t);
        e.H(0xc65b0, t, 0, 0, 0, 0, u);
        return 1;
    }
    e.H(0xc60a, 0, 0, 0, 0, 0, t, 0x1000);
    return res;
}
