// 1.13c D2Client+0xA25C0: ESI = p (request block), one stack argument n, ret 4. Emits the ring pattern of (n - 1) requests through
// 0xB30F0 on p: the first at (+0xE, -0xE), then per ring slot a (mirrored pair x inner-loop) group and one axis point.
#include "cl_env.h"
extern "C" void d2cl_a25c0(uint32_t p, int32_t n, const cl::Env& e) {
    static const int32_t T1F8[7] = { 0, 0, 0x14, -0x14, 0xE, 0xE, -0xE };
    static const int32_t T218[7] = { 0x14, -0x14, 0, 0, 0xE, -0xE, -0xE };
    static const int32_t T238[7] = { 8, 2, 0xB, 4, 0xD, 6, 9 };
    static const int32_t T254[7] = { 0x12, 0x14, 0x11, 0x14, 0xF, 0x13, 0x12 };
    static const int32_t T270[4] = { -1, -1, 1, 1 };
    static const int32_t T280[4] = { -1, 1, 1, -1 };
    int32_t r = n - 1;
    if (r / 5 > 7) __builtin_trap();   // retail pushes 0x848 and exits (fatal path; never captured)
    e.W(p + 0x1c, 0xE); e.W(p + 0x20, (uint32_t)-0xE);
    e.HB(0xb30f0, p);
    int32_t a = 0;
    do {
        if (r == 0) return;
        int32_t k = 0;
        while (k < 4 && r > 0) {
            e.W(p + 0x1c, (uint32_t)(T280[k] * T254[a])); e.W(p + 0x20, (uint32_t)(T270[k] * T238[a]));
            e.HB(0xb30f0, p);
            e.W(p + 0x1c, (uint32_t)(T280[k] * T238[a])); e.W(p + 0x20, (uint32_t)(T270[k] * T254[a]));
            e.HB(0xb30f0, p);
            ++k; r -= 2;
        }
        e.W(p + 0x1c, (uint32_t)T218[a]); e.W(p + 0x20, (uint32_t)T1F8[a]);
        e.HB(0xb30f0, p);
        ++a; --r;
    } while (a < 7);
}
