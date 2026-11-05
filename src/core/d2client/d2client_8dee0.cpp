// FN probe entry for 1.13c D2Client+0x8DEE0 (src/core/fn/d2client_8dee0.cpp): seams are the D2Lang string getter and the
// D2Common stat query #10973 (thunk 0xC1C6); the port itself is d2client_batch_8d940.cpp.
#include "d2client_8d940_env.h"
extern "C" void d2c_port_8dee0(int32_t unit, const uint8_t* rec, uint16_t* out, const uint16_t* (*str)(uint32_t id), int32_t (*q)(int32_t unit, int32_t stat)) {
    struct E final : d2client_batch_8d940::TrapEnv {
        const uint16_t* (*s)(uint32_t); int32_t (*qq)(int32_t, int32_t);
        E(const uint16_t* (*f)(uint32_t), int32_t (*g)(int32_t, int32_t)) : s(f), qq(g) {}
        const d2client_batch_8d940::wc* str(int32_t id) override { return s((uint32_t)id); }
        int32_t q(int32_t what, int32_t a, int32_t b) override { (void)what; (void)b; return qq(a, b); }
    } e(str, q);
    d2client_batch_8d940::tooltipStat46(e, unit, rec, out);
}
