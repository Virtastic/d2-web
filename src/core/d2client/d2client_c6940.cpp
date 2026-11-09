// 1.13c D2Client+0xC6940, written from the disassembly. ECX = unit, one unused stack word (ret 4); returns 1. Flags f = [unit+0x38] & ~0xFF.
// f < 0x600 (signed): m = 0xC7A8(ECX = unit) & 0xF; m >= 14 is the fatal path (0xBF92(0xB60), 0xBF80(file tag, that), 0x3664(-1)) whose result is taken as the path;
// else 0x88A70(EBX = word [0xF5538 + 8m]; unit, 4, 0, 0), 0x2A0B0(EAX = unit), and a path [unit+0x2C] (null: the same fatal path with 0x946). With a path:
// o = 0xC2DA(path); none or a null [unit+0x2C] returns 1, else 0xC478(path, 0x1630(o), 0x1660(o), 0). f >= 0x600: 0x2A0B0(EAX = unit), the path as above (null: fatal with 0x946);
// o = 0xC2DA(path), k = o ? [o+0xC] : -1, X = {0, -1, 0, k, k, 0}; 0xA1860(unit, 0, 0, 3, 0xA783, module 0xA09F0, &X, 0) fills X; o2 = X[0] or X[2] when X[0] is 0;
// o2 and [unit+0x2C] non-zero: 0xC478([unit+0x2C], 0x1630(o2), 0x1660(o2), 0). Both the 0x1660 call and its result precede 0x1630 (call order).
// Fatal path with the A-side path from 3664 continues into the f >= 0x600 body's 0xC2DA step.
#include "cl_mb2.h"
static uint32_t fatal(const clm::Env& e, uint32_t code) {
    const uint32_t s = e.H(0xbf92, 0, 0, 0, 0, 0, code);
    e.H(0xbf80, 0, 0, 0, 0, 0, clm::Env::Mod(0xd359f), s);
    return e.H(0x3664, 0, 0, 0, 0, 0, 0xffffffffu);
}
static void place(const clm::Env& e, uint32_t u, uint32_t o) {
    const uint32_t p = e.R(u + 0x2c);
    const uint32_t hgt = e.H(0x1660, 0, o);
    const uint32_t wid = e.H(0x1630, 0, o);
    e.H(0xc478, 0, 0, 0, 0, 0, p, wid, hgt, 0);
}
// 1.13c D2Client+0xC6940
extern "C" uint32_t d2c_c6940(uint32_t u, uint32_t, const clm::Env& e) {
    uint32_t path;
    bool bodyB = (int32_t)(e.R(u + 0x38) & 0xffffff00u) >= 0x600;
    if (!bodyB) {
        const uint32_t m = e.H(0xc7a8, 0, u) & 0xf;
        if ((int32_t)m >= 14) {
            path = fatal(e, 0xb60);
        } else {
            e.H(0x88a70, 0, 0, 0, e.G(0xf5538 + 8 * m), 0, u, 4, 0, 0);
            e.H(0x2a0b0, u);
            path = e.R(u + 0x2c);
            if (path) {
                const uint32_t o = e.H(0xc2da, 0, 0, 0, 0, 0, path);
                if (!o) return 1;
                if (!e.R(u + 0x2c)) return 1;
                place(e, u, o);
                return 1;
            }
            path = fatal(e, 0x946);
        }
    } else {
        e.H(0x2a0b0, u);
        path = e.R(u + 0x2c);
        if (!path) path = fatal(e, 0x946);
    }
    const uint32_t o = e.H(0xc2da, 0, 0, 0, 0, 0, path);
    const uint32_t k = o ? e.R(o + 0xc) : 0xffffffffu;
    uint32_t X[6] = { 0, 0xffffffffu, 0, k, k, 0 };
    e.H(0xa1860, 0, 0, 0, 0, 0, u, 0, 0, 3, 0xa783, clm::Env::Mod(0xa09f0), (uint32_t)(uintptr_t)X);
    uint32_t o2 = X[0];
    if (!o2) { o2 = X[2]; X[0] = o2; }
    if (!o2) return 1;
    if (!e.R(u + 0x2c)) return 1;
    place(e, u, o2);
    return 1;
}
