// D2Client+0xBE240: register-arg (ESI=mode) string-id picker; env-seam style. Ordinals unverified.
#include <cstdint>
#include <emscripten.h>
struct D2cBE240Env {
    int32_t (*ord10014)();                 // Ordinal_10014 (D2Client+0xC214 thunk; unverified name)
    int32_t (*ord10003)();                 // Ordinal_10003 (D2Client+0xBF80 thunk; unverified name)
    int32_t (*ord10265)(int32_t id);       // Ordinal_10265 (+0xBF92 thunk; unverified)
    void (*ord10024)(const void* msg, int32_t s);  // Ordinal_10024 (+0x3664/0xD1EC; unverified)
    void (*exit_)(int32_t code);           // _exit(-1)
    int32_t dat_cc0a0;                     // DAT_6fbcc0a0
    uint8_t dat_c5591;                     // DAT_6fbc5591 (both branches call ord10003 identically)
    const void* msg_8359f;                 // &DAT_6fb8359f
};
extern "C" {
// 1.13c D2Client+0xBE240
EMSCRIPTEN_KEEPALIVE int32_t d2c_be240(const D2cBE240Env* e, int32_t esi) {
    int32_t r = e->ord10014();
    if (r == 0) return e->ord10003();
    if (esi != 8) return r + 0x16e;
    if (e->dat_cc0a0 == 0) { e->ord10024(e->msg_8359f, e->ord10265(0x60)); e->exit_(-1); }
    return e->ord10003();
}
}
