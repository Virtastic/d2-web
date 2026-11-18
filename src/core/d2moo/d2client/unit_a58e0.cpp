// D2Client sub_A58E0: unit (EAX) -> *(*(unit+0x54)+0x38); asserts otherwise. Env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA58E0Env {
    uint32_t (*ord10265)(uint32_t line);                 // Ordinal_10265 (D2Lang/assert text id)
    void (*ord10024)(const void* fmt, uint32_t arg);     // Ordinal_10024 (assert report)
    const void* g_6fb8359f;                              // D2Client+0x8359F string
    void (*exit_)(int32_t code);                         // _exit(-1)
    const uint8_t* mem;                                  // base for 32-bit retail addresses
};
extern "C" {
// 1.13c D2Client+0xA58E0
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a58e0(const D2cA58E0Env* e, uint32_t unit) {
    uint32_t id = 0x57e;
    if (unit) {
        uint32_t p = *(const uint32_t*)(e->mem + unit + 0x54);
        if (p) return *(const uint32_t*)(e->mem + p + 0x38);
        id = 0xd0a;
    }
    e->ord10024(e->g_6fb8359f, e->ord10265(id));
    e->exit_(-1);
    return 0;
}
}
