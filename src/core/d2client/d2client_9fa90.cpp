// Panel close. Retail: no arguments, plain ret. A non-zero state word 0x10B214 first calls 0x9F9E0 (EAX = the state). Then for each of the handles
// 0x11BC0C and 0x11BC10: when non-null, Ordinal 10065 (thunk 0xC18A, one stack word, the handle) and 0xBF86 (ECX = handle, EDX = module string 0xD6AB0,
// stack words 0x7A, 0; stdcall); the handle word is then cleared.
// 1.13c D2Client+0x9FA90
#include <cstdint>
#include <emscripten.h>

struct D2cRow9fa90Env {
    uint32_t* st;                                                              // D2Client+0x10B214 (state word)
    uint32_t* h;                                                               // D2Client+0x11BC0C (2 words)
    uint32_t str;                                                              // EDX of the 0xBF86 call (module string 0xD6AB0)
    void (*c9f9e0)(uint32_t eax);                                              // 0x9F9E0
    void (*cc18a)(uint32_t handle);                                            // 0xC18A
    void (*cbf86)(uint32_t ecx, uint32_t edx, uint32_t a, uint32_t b);         // 0xBF86
};

extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_9fa90(const D2cRow9fa90Env* e) {
    if (e->st[0]) e->c9f9e0(e->st[0]);
    for (int i = 0; i < 2; ++i) {
        const uint32_t v = e->h[i];
        if (!v) continue;
        e->cc18a(v);
        e->cbf86(v, e->str, 0x7a, 0);
        e->h[i] = 0;
    }
}
