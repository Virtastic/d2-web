// D2Client+0xA6070: ESI=dword array; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA6070Env {
    uint32_t (*ord10828)();            // Ordinal_10828
    uint32_t (*ord10507)();            // Ordinal_10507
    int32_t (*ord10601)(uint32_t);     // Ordinal_10601 (result -> EAX)
    uint32_t (*fA56C0)(int32_t eax);   // D2Client+0xA56C0
};
extern "C" {
// 1.13c D2Client+0xA6070
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a6070(const D2cA6070Env* e, const uint32_t* esi) {
    if (!esi || esi[0] != 1) return 0;
    if (e->fA56C0(e->ord10601(e->ord10828()))) return 1;
    return e->fA56C0(e->ord10601(e->ord10507())) != 0;
}
}
