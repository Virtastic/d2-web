// 1.13c D2Client+0x3C5B0 -- slot-pass sibling with the toggle word 0xFAD88 (object state at +8) and the panel slide tail. No
// arguments. The old toggle word is read first. While it is 0 (and the pending word 0x113A68 is clear) each active slot (array
// 0xFAD80 == 1) acts on its state: 0 nothing, 1 the request routine 0xC2790 (ECX = slot, EDX = 1, one zero stack word), 4 the
// routine 0x47B20 (EDI = the word 0x11971D) when 0x119721 is set; 2, 3 and above 4 leave at once with the state in EAX (the pending
// word leaves with the answer of 0xBF8C). The toggle word then becomes (word == 0) unless the local player (0x11BBFC) has a nonzero
// first dword or 0xA5440 answers nonzero. Old 0 and new nonzero: 0x3CCC0 (EAX = 2, handle of string 0xD0628 from 0xD0B4); old nonzero
// and new 0: 0xBF720, then 0x3CC60 the same way, then 0xBE2E0. The slide tail then moves the viewport: with 0xFAD84 and 0xFAD90 both
// clear and the toggle nonzero the slide amount 0x11C418 = width/4 (0xDBC48), mode 0x11C414 = 2, 0xD1B0 (ECX = amount), 0x8A4D0 (EAX
// = 0xF9E18, ECX = 0xF9E14 + a quarter, EDX = 0, ESI = 0x11C3C4, stack = the quarter of 0xF9E14); with the toggle zero amount 0, mode
// 0 and 0x8A4D0 with the plain values. Either then sets the cursor (0x161B0, EBX = x, EDI = y) when the cursor x (0x11B828) is inside
// the shifted half. With 0xFAD84 or 0xFAD90 set: toggle nonzero -> amount 0, mode 3, 0x8A4D0 plain and the four latches cleared;
// toggle zero -> amount -(width/4), mode 1, 0x8A4D0 (ECX = 0xF9E14 - quarter, stack = -quarter). The assert path exits.
#include "cl_pa4.h"
namespace {
inline int32_t qd4(int32_t v) { return (v + ((v >> 31) & 3)) >> 2; }
}
extern "C" uint32_t d2c_3c5b0(const cl::Env& e) {
    using namespace pa4;
    const uint32_t ebx = G(e, 0xfad88);
    uint32_t eax = e.H(0xbf8c);
    if (G(e, 0xdd5d0) != 0x26) {
        const uint32_t r = e.H(0xbf92, 0x139b);
        e.H(0xbf80, 0x6FB8359Fu, r, 0x139b);
        eax = e.H(0x3664, 0xFFFFFFFFu);
    }
    uint32_t edx = G(e, 0xfad88);
    if (edx == 0) {
        if (G(e, 0x113a68)) return eax;
        uint32_t i = 0;
        do {
            if (G(e, 0xFAD80 + 4 * i) == 1) {
                eax = e.R(G(e, 0xDD538 + 4 * i) + 0x8);
                if (eax > 4 || eax == 2 || eax == 3) return eax;
                if (eax == 1) {
                    eax = e.H(0xc2790, 0, 0, 0, 0, 0, 0, i, 1);
                    edx = G(e, 0xfad88);
                } else if (eax == 4) {
                    if (G(e, 0x119721)) {
                        eax = e.H(0x47b20);
                        e.Ej(0x47b20, G(e, 0x11971d));
                        edx = G(e, 0xfad88);
                    }
                }
            }
            eax = G(e, 0xdd5d0);
            ++i;
        } while ((int32_t)i < (int32_t)eax);
    }
    eax = G(e, 0x11bbfc);
    if (eax) {
        if (e.R(eax) == 0) {
            eax = e.H(0xa5440);
            edx = eax;   // the recorder thunk leaves EDX = the hook's return value
            if (eax == 0) { edx = (edx == 0); S(e, 0xfad88, edx); }
        }
    } else {
        eax = (edx == 0);
        edx = eax;
        S(e, 0xfad88, edx);
    }
    const uint32_t esi = edx;
    if (ebx == 0) {
        if (edx != 0) {
            const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0628);
            eax = e.H(0x3ccc0, h, 0, 0, 0, 0, 2);
        }
    } else if (edx == 0) {
        e.H(0xbf720);
        const uint32_t h = e.H(0xd0b4, 0xB2000000u | 0xD0628);
        e.H(0x3cc60, h, 0, 0, 0, 0, 2);
        eax = e.H(0xbe2e0);
    }
    const int32_t cx = (int32_t)G(e, 0x11b828);
    const uint32_t cy = G(e, 0x11b824);
    const int32_t w = (int32_t)G(e, 0xdbc48);
    if (G(e, 0xfad84) != 0 || G(e, 0xfad90) != 0) {
        if (esi != 0) {
            S(e, 0x11c418, 0);
            S(e, 0x11c414, 3);
            e.H(0xd1b0, 0, 0, 0, 0, 0, 0, 0);
            eax = e.H(0x8a4d0, 0, 0, 0, 0, 0, G(e, 0xf9e18), G(e, 0xf9e14), 0, 0, G(e, 0x11c3c4));
            S(e, 0x11c3dc, 0);
            S(e, 0x11c3e0, 0);
            S(e, 0x11c3f8, 0);
            S(e, 0x11c3fc, 0);
            return eax;
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
