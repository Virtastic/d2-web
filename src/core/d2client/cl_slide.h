// The panel-slide tail shared by the right-hand panel toggles 0x3B5B0 and 0x3C2A0 (1.13c D2Client): the viewport slide driven by
// the new toggle word (esi) and the seven flag words 0xFAD88, 0xFADD0, 0xFADBC, 0xFADC0, 0xFAE10, 0xFAE14, 0xFADD8. Returns what
// the function leaves in EAX. cx / cy = the cursor (0x11B828 / 0x11B824) read before the tail.
#pragma once
#include "cl_pa4.h"
namespace pa4 {
inline int32_t qd4(int32_t v) { return (v + ((v >> 31) & 3)) >> 2; }
inline uint32_t slide7(const cl::Env& e, uint32_t esi, int32_t cx, uint32_t cy) {
    static const uint32_t flags[7] = {0xfad88, 0xfadd0, 0xfadbc, 0xfadc0, 0xfae10, 0xfae14, 0xfadd8};
    bool any = false;
    for (uint32_t f : flags) any = any || G(e, f) != 0;
    const int32_t w = (int32_t)G(e, 0xdbc48);
    if (any) {
        if (esi != 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 3);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            const uint32_t r = e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            S(e, 0x11c3dc, 0);
            S(e, 0x11c3e0, 0);
            S(e, 0x11c3f8, 0);
            S(e, 0x11c3fc, 0);
            return r;
        }
        const int32_t q = qd4(w);
        S(e, 0x11c418, (uint32_t)q);
        S(e, 0x11c414, 2);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
        const int32_t f = (int32_t)G(e, 0xf9e14), q2 = qd4(f);
        return e.H(0x8a4d0, (uint32_t)q2, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(f + q2), 0, 0, G(e, 0x11c3c4));
    }
    if (esi != 0) {
        const int32_t q = -qd4(w);
        S(e, 0x11c418, (uint32_t)q);
        S(e, 0x11c414, 1);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
        const int32_t f = (int32_t)G(e, 0xf9e14), q2 = qd4(f);
        e.H(0x8a4d0, (uint32_t)-q2, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(f - q2), 0, 0, G(e, 0x11c3c4));
        const int32_t w2 = (int32_t)G(e, 0xdbc48), q3 = qd4(w2);
        if (cx <= q3) return (uint32_t)q3;
        const uint32_t r = e.H(0x161b0, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(cx - q3));
        e.Ej(0x161b0, cy);
        return r;
    }
    S(e, 0x11c418, 0);
    S(e, 0x11c414, 0);
    e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
    e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
    const int32_t w2 = (int32_t)G(e, 0xdbc48);
    const int32_t half = (w2 - (w2 >> 31)) >> 1;
    if (cx >= half) return (uint32_t)half;
    const uint32_t r = e.H(0x161b0, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(qd4(w2) + cx));
    e.Ej(0x161b0, cy);
    return r;
}
// The left-hand panel slide tail of 0x3C5B0 and 0x3BFC0 (the flag words are 0xFAD84 and 0xFAD90; polarity opposite to slide7).
inline uint32_t slideA(const cl::Env& e, uint32_t esi, int32_t cx, uint32_t cy) {
    const int32_t w = (int32_t)G(e, 0xdbc48);
    if (G(e, 0xfad84) != 0 || G(e, 0xfad90) != 0) {
        if (esi != 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 3);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            const uint32_t r = e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            S(e, 0x11c3dc, 0);
            S(e, 0x11c3e0, 0);
            S(e, 0x11c3f8, 0);
            S(e, 0x11c3fc, 0);
            return r;
        }
        const int32_t q = -qd4(w);
        S(e, 0x11c418, (uint32_t)q);
        S(e, 0x11c414, 1);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
        const int32_t f = (int32_t)G(e, 0xf9e14), q2 = qd4(f);
        return e.H(0x8a4d0, (uint32_t)-q2, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(f - q2), 0, 0, G(e, 0x11c3c4));
    }
    if (esi != 0) {
        const int32_t q = qd4(w);
        S(e, 0x11c418, (uint32_t)q);
        S(e, 0x11c414, 2);
        e.H(0xd1b0, 0, 0, 0, 0, 0, 0, (uint32_t)q);
        const int32_t f = (int32_t)G(e, 0xf9e14), q2 = qd4(f);
        e.H(0x8a4d0, (uint32_t)q2, 0, 0, 0, 0, G(e, 0xf9e18), (uint32_t)(f + q2), 0, 0, G(e, 0x11c3c4));
        const int32_t w2 = (int32_t)G(e, 0xdbc48), q3 = qd4(w2);
        if (cx >= w2 - q3) return (uint32_t)q3;
        const uint32_t r = e.H(0x161b0, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(cx + q3));
        e.Ej(0x161b0, cy);
        return r;
    }
    S(e, 0x11c418, 0);
    S(e, 0x11c414, 0);
    e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
    e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
    const int32_t w2 = (int32_t)G(e, 0xdbc48);
    const int32_t half = (w2 - (w2 >> 31)) >> 1;
    if (cx <= half) return (uint32_t)half;
    const uint32_t r = e.H(0x161b0, 0, 0, 0, 0, 0, 0, 0, 0, (uint32_t)(cx - qd4(w2)));
    e.Ej(0x161b0, cy);
    return r;
}
}  // namespace pa4
