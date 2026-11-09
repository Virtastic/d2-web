// 1.13c D2Client+0x8AFD0 (229 B, stdcall, ret 8): screen_shake_offset. Stack a0/a1 = int* x, y.
// ga = window 0xBB9CC..0xBB9DF: [0] start tick, [1] ramp-up ms, [2] hold ms, [3] ramp-down ms, [4] last y jitter (0xBB9DC).
// gu = window 0x11BBFC (player unit; its +0x20 is the seed the 0x18D0 range-random takes in ECX, ESI = 2*amp, EDI = -amp).
// gs = window 0x11BEFC..0x11BF03: [0] peak amplitude, [1] last x jitter. 0x8AD30 takes EAX = the amplitude (0 when expired
// or when the ramp it falls in has zero length). The ramp-down is the unsigned 32-bit imul/div retail emits. now = GetTickCount.
#include "cl_env.h"
extern "C" void d2c_8afd0(uint32_t* px, uint32_t* py, uint32_t* ga, const uint32_t* gu, uint32_t* gs, uint32_t now,
                          const cl::Env& e) {
    const uint32_t t = now - ga[0], up = ga[1], hold = ga[2], down = ga[3];
    if (t > down + hold + up) {
        e.H(0x8ad30, 0, 0, 0, 0, 0, 0);
        gs[0] = 0; gs[1] = 0; ga[4] = 0;
        return;
    }
    uint32_t amp = 0;
    if (t < up) {
        if (up) amp = gs[0] * t / up;
    } else if (t < hold + up) {
        amp = gs[0];
    } else if (down) {
        amp = (down - t + hold + up) * gs[0] / down;
    }
    if (amp) {
        const uint32_t seed = gu[0] + 0x20;
        gs[1] = e.H(0x18d0, 0, 0, 0, 0, 0, 0, seed, 0, 0, amp * 2); e.Ej(0x18d0, 0u - amp);
        const uint32_t jy = e.H(0x18d0, 0, 0, 0, 0, 0, 0, seed, 0, 0, amp * 2); e.Ej(0x18d0, 0u - amp);
        *px += gs[1];
        ga[4] = jy;
        *py += jy;
    }
    e.H(0x8ad30, 0, 0, 0, 0, 0, amp);
}
