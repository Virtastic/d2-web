// FN probe entry for 1.13c D2Client+0x8D940 (src/core/fn/d2client_8d940.cpp): the seams are the D2Lang string getter, the D2Common queries
// #10744 / #10494 (Env::q) and the skill calls (Imp8d940); the port itself is d2client_batch_8d940.cpp.
#include "d2client_8d940_env.h"
extern "C" void d2c_port_8d940(int32_t player, int32_t unit, const uint8_t* data, uint16_t* out, const uint16_t* (*str)(uint32_t id),
                               int32_t (*q)(int32_t what, int32_t a, int32_t b),
                               int32_t (*o10630)(int32_t, int32_t), int32_t (*o10306)(int32_t, int32_t, int32_t),
                               int32_t (*o10567)(int32_t, int32_t, int32_t, int32_t), int32_t (*o10297)(int32_t, int32_t, int32_t, int32_t)) {
    struct E final : d2client_batch_8d940::TrapEnv {
        const uint16_t* (*s)(uint32_t); int32_t (*qq)(int32_t, int32_t, int32_t);
        E(const uint16_t* (*f)(uint32_t), int32_t (*g)(int32_t, int32_t, int32_t)) : s(f), qq(g) {}
        const d2client_batch_8d940::wc* str(int32_t id) override { return s((uint32_t)id); }
        int32_t q(int32_t what, int32_t a, int32_t b) override { return qq(what, a, b); }
    } e(str, q);
    const d2client_batch_8d940::Imp8d940 f{ o10630, o10306, o10567, o10297 };
    d2client_batch_8d940::tooltipRequirement(e, f, player, unit, data, out);
}
