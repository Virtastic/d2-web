// 1.13c D2Client+0x6F3E0 (191 B, caller 0x70140): EAX = x, stack y; the two chat segments (A = the name at 0x11EC40,
// B = the message at 0x11FE98), each wrapped to x = 0x83, y + 0xF when it would pass the right edge ([0xDBC48] - 0x109).
// 0xD35A measures (ECX = string), 0xD37E draws (ECX = string, EDX = x, stack y, 4, 0); B is drawn at x + the width of A.
// Ends with 0x6EF20 (ECX = y, EDX = x + both widths - 0x83, stack 4) and returns the EAX it leaves.
#include "client_host.h"
#include "d2client_hw.h"
namespace {
uint32_t wlen16(const uint16_t* s) { uint32_t n = 0; while (s[n]) ++n; return n; }
}
// 1.13c D2Client+0x6F3E0
extern "C" uint32_t d2c_6f3e0(int32_t x, int32_t y, const uint16_t* a, const uint16_t* b, int32_t rightEdge, D2cHost h) {
    const uint32_t pa = (uint32_t)(uintptr_t)a, pb = (uint32_t)(uintptr_t)b;
    int32_t w1 = 0;
    if (wlen16(a)) {
        w1 = (int32_t)hc(h, 0xd35a, 0, pa);
        if (w1 + x - 0x83 > rightEdge - 0x109) { x = 0x83; y += 0xf; }
        hc(h, 0xd37e, 0, pa, (uint32_t)x, 0, 0, (uint32_t)y, 4, 0);
    }
    const int32_t w2 = (int32_t)hc(h, 0xd35a, 0, pb);
    if (wlen16(b)) {
        if (x + w2 + w1 - 0x83 > rightEdge - 0x109) { x = 0x83; y += 0xf; }
        hc(h, 0xd37e, 0, pb, (uint32_t)(x + w1), 0, 0, (uint32_t)y, 4, 0);
    }
    return hc(h, 0x6ef20, 0, (uint32_t)y, (uint32_t)(w1 + w2 + x - 0x83), 0, 0, 4);
}
