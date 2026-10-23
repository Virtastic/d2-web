// D2Client sub_A1340: unit size (w,h) lookup, env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA1340Env {
    void (*fada0b0)();                          // FUN_6fada0b0 (prologue helper)
    uint32_t (*ord10265)(uint32_t id);          // string by id
    void (*ord10024)(const void* fmt, uint32_t s);
    const void* g_6fb8359f;
    const void* (*ord10392)(uint32_t unitId);   // unit lookup -> 32-bit-layout struct
    int32_t (*ord10983)(uint32_t unitId);
    int32_t (*ord10764)(uint32_t unitId);
    int32_t (*ord10867)(uint32_t p);
    int32_t (*ord10750)(uint32_t p);
    void (*f50b30)(uint32_t* w, uint32_t* h);   // D2Client+0xA0B30 fallback
    void (*exit_)(int32_t);
};
extern "C" {
// 1.13c D2Client+0xA1340
// self = EBX object (u32 at +0x2c = unit id); outW = EDI; outH = param_1. Returns 1.
EMSCRIPTEN_KEEPALIVE uint32_t d2c_a1340(const D2cA1340Env* e, const uint8_t* self, int32_t* outW, int32_t* outH) {
    e->fada0b0();
    uint32_t id = *(const uint32_t*)(self + 0x2c);
    if (id == 0) {
        e->ord10024(e->g_6fb8359f, e->ord10265(0x946));
        e->exit_(-1);
        return 1;
    }
    const uint32_t* u = (const uint32_t*)e->ord10392(id);
    int32_t h;
    if (!u) {
        *outW = e->ord10983(id);
        h = e->ord10764(id);
    } else {
        int32_t t = (int32_t)u[0];
        uint32_t p = u[0xb];
        bool direct = t == 2 || (t > 3 && t < 6);
        *outW = direct ? (int32_t)*(const uint32_t*)(uintptr_t)(p + 0xc) : p ? e->ord10867(p) : 0;
        h = direct ? (int32_t)*(const uint32_t*)(uintptr_t)(p + 0x10) : p ? e->ord10750(p) : 0;
    }
    *outH = h;
    if (*outW == 0 || *outH == 0) e->f50b30((uint32_t*)outW, (uint32_t*)outH);
    return 1;
}
}
