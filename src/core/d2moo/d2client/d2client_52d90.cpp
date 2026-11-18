// 1.13c D2Client+0x52D90 -- the "n seconds" countdown line: no inputs. g = a pinned globals block (+0 deadline [0x11C2E8], +4 id [0xF2134],
// +8 screen width [0xDBC48], +0x20 the 128-slot unit hash table [0x10A808]). Tick = 0xC09A(). Nothing is drawn without a deadline or at/after it.
// The unit with guid == id (hash chain through [u+0xE4]) must be type 1 (else the fatal path 0xBF92/0xBF80/0x3664, never vectored); unless
// its mode [u+0x10] is 0xC the line only shows in the last 0x46 ticks. Draw: 256 Unicode constructors over a local, the string 0x13E9 through
// 0xD1EC(remaining), the donor sprintf(0x100, buf, fmt, remaining), font 0xD swapped in (0xD3A8), x = screenW/2 - width/2 (round toward zero),
// 0xD37E(buf at x; 0x177, 9, 1), font restored.
#include "cl_env.h"
extern "C" void d2c_52d90(const cl::Env& e, uint32_t g) {
    int32_t tick = (int32_t)e.H(0xc09a), dl = (int32_t)e.R(g);
    if (dl == 0 || dl <= tick) return;
    uint32_t id = e.R(g + 4);
    for (uint32_t p = e.R(g + 0x20 + 4 * (id & 0x7f)); p; p = e.R(p + 0xe4)) {
        if (e.R(p + 0xc) != id) continue;
        if (e.R(p) != 1) {
            uint32_t m = e.H(0xbf92, 0x5a);
            e.H(0xbf80, 0xd359f, m);
            e.H(0x3664, 0xffffffffu);
            return;
        }
        if (e.R(p + 0x10) != 0xc && tick < dl - 0x46) return;
        break;
    }
    volatile uint16_t buf[256];
    uint32_t b = (uint32_t)(uintptr_t)&buf[0];
    for (uint32_t i = 0; i < 256; i++) e.H(0x11070, 0, 0, 0, 0, 0, 0, b + 2 * i);
    uint32_t rem = (uint32_t)(dl - tick);
    uint32_t fmt = e.H(0xd1ec, rem, 0, 0, 0, 0, 0, 0x13e9);
    e.H(0x119c0, 0x100, b, fmt, rem);
    uint32_t old = e.H(0xd3a8, 0, 0, 0, 0, 0, 0, 0xd);
    int32_t w = (int32_t)e.H(0xd35a, 0, 0, 0, 0, 0, 0, b), sw = (int32_t)e.R(g + 8);
    int32_t x = ((sw - (sw >> 31)) >> 1) - ((w - (w >> 31)) >> 1);
    e.H(0xd37e, 0x177, 9, 1, 0, 0, 0, b, (uint32_t)x);
    e.H(0xd3a8, 0, 0, 0, 0, 0, 0, old);
}
