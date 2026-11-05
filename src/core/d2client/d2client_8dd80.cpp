// FN probe entry for 1.13c D2Client+0x8DD80 (src/core/fn/d2client_8dd80.cpp): the D2Lang string getter is the only seam; Storm _snprintf and
// the D2Lang Unicode copies run as the port's own helpers. The port itself is d2client_batch_8d940.cpp.
#include "d2client_8d940_env.h"
extern "C" void d2c_port_8dd80(const uint8_t* data, int32_t add, uint16_t* out, const uint16_t* (*str)(uint32_t id)) {
    struct E final : d2client_batch_8d940::TrapEnv {
        const uint16_t* (*s)(uint32_t); explicit E(const uint16_t* (*f)(uint32_t)) : s(f) {}
        const d2client_batch_8d940::wc* str(int32_t id) override { return s((uint32_t)id); }
    } e(str);
    d2client_batch_8d940::tooltipField10a(e, data, add, out);
}
