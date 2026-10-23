// D2Client+0xA5F70: register-arg helper (ESI=struct base); env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA5F70Env {
    void (*f2A0B0)();                    // D2Client+0x2A0B0
    uint32_t (*ord10265)(uint32_t id);   // Ordinal_10265 (string lookup; ordinal unverified)
    void (*ord10024)(uint32_t str);      // Ordinal_10024 (fatal-error report w/ DAT 0xB359F; unverified)
    void (*ord10392)(uint32_t arg);      // Ordinal_10392 (unverified)
    void (*exit_)(int32_t code);         // D2Client+0x3664 _exit
};
extern "C" {
// 1.13c D2Client+0xA5F70
EMSCRIPTEN_KEEPALIVE void d2c_a5f70(const D2cA5F70Env* e, const uint8_t* esi) {
    e->f2A0B0();
    uint32_t v = *(const uint32_t*)(esi + 0x2c);
    if (v == 0) {
        e->ord10024(e->ord10265(0x946));
        e->exit_(-1);
        return;
    }
    e->ord10392(v);
}
}
