// D2Client+0xA53A0: EAX = obj; null -> fatal assert (line 0x96c); else 0xA4DA0(table 0x10AA08+0x...). env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA53A0Env {
    void (*fatal)(uint32_t line);        // Ordinal_10265(0x96c) -> Ordinal_10024(str 0x7359f RVA) -> _exit(-1)
    void (*a4da0)(uint32_t table_rva);   // D2Client+0xA4DA0
};
extern "C" {
// 1.13c D2Client+0xA53A0 (callers 0x1E1D2, 0x1E160)
EMSCRIPTEN_KEEPALIVE void d2c_a53a0(uint32_t eax, const D2cA53A0Env* e) {
    if (!eax) { e->fatal(0x96c); return; }
    e->a4da0(0x10A808);  // 0x6FBBA808 - 0x6FAB0000
}
}
