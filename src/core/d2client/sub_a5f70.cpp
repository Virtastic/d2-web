// D2Client+0xA5F70: register-arg helper (ESI = struct base); env-seam style.
// Retail: EAX = ESI into D2Client+0x2A0B0 (validates the struct), then the dword at +0x2C goes to import thunk 0xC2DA (stdcall 1) whose result is returned.
#include <cstdint>
#include <emscripten.h>
struct D2cA5F70Env {
    void (*f2A0B0)(uint32_t eax);        // D2Client+0x2A0B0 (EAX = struct)
    uint32_t (*ord10265)(uint32_t id);   // import thunk 0xBF92 (string lookup)
    void (*ord10024)(uint32_t str);      // import thunk 0xBF80 (fatal-error report w/ DAT 0xB359F)
    uint32_t (*ord10392)(uint32_t arg);  // import thunk 0xC2DA (stdcall 1)
    void (*exit_)(int32_t code);         // D2Client+0x3664 _exit
};
extern "C" {
// 1.13c D2Client+0xA5F70
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a5f70(const D2cA5F70Env* e, const uint8_t* esi) {
    e->f2A0B0((uint32_t)(uintptr_t)esi);
    uint32_t v = *(const uint32_t*)(esi + 0x2c);
    if (v == 0) {
        e->ord10024(e->ord10265(0x946));
        e->exit_(-1);
        return 0;
    }
    return e->ord10392(v);
}
}
