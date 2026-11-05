// EAX = unit; 0 for null, 1 for a non-player type, else
// the hand-item test over inventory slots 4 and 5 (10048 / 10711 / stat 0x7D) returning 10104 of the first match.
#include <cstdint>
#include "cl_env.h"
// 1.13c D2Client+0xA0A30
extern "C" uint32_t d2c_a0a30(uint32_t u, const cl::Env& e) {
    if (!u) return 0;
    if (e.R(u) != 0) return 1;
    const uint32_t p = e.H(0xc6ee, u, 0);                           // 10641
    if (!p) return 1;
    bool must = true;   // EBP: the slot item must be the 10641 item
    if (e.H(0xc772, p)) {                                           // 10543
        must = false;
        const uint32_t c = e.H(0xc310, u, 0xB2000000u | 0xd44b4u, 0xb6);   // 10511
        if (!e.H(0xc42a, c) && e.H(0xc454, p, 0x9d, 0)) return 1;  // 10601, 10910
    }
    const uint32_t inv = e.R(u + 0x60);
    if (!inv) return 0;
    for (uint32_t slot = 4; slot <= 5; ++slot) {
        const uint32_t x = e.H(0xc262, inv, slot);                  // 11139
        if (!x || (must && x != p)) continue;
        if (e.H(0xc73c, x) || e.H(0xc76c, x) || e.H(0xc454, x, 0x7d, 0)) return e.H(0xc7ba, x);
    }
    return 0;
}
