// D2Client+0x9F950: state toggle 1<->2 (EAX=arg ptr), env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2c9F950Env {
    uint32_t* g_bbb214;  // DAT_6fbbb214 state (RVA 0xBB214)
    uint32_t* g_bbb220;  // DAT_6fbbb220
    uint32_t* g_cb834;   // DAT_6fbcb834
    uint32_t* g_cb864;   // DAT_6fbcb864
    uint32_t* g_cb854;   // DAT_6fbcb854
    uint32_t* g_cb858;   // DAT_6fbcb858
    const uint8_t* arg;  // EAX: +0xC read as uint32_t (host-side view)
    void (*f81E40)();
    void (*f162D0)();
    void (*f88A70)(int32_t a, int32_t b, int32_t c, int32_t d);
    int32_t edx_after_162D0;  // extraout_EDX of 0x162D0
};
extern "C" {
// 1.13c D2Client+0x9F950
EMSCRIPTEN_KEEPALIVE void d2c_9f950(const D2c9F950Env* e) {
    if (*e->g_bbb214 == 1) {
        *e->g_bbb214 = 2; *e->g_cb834 = 0; *e->g_cb864 = 1; *e->g_cb854 = 5; *e->g_cb858 = 0;
        if (e->arg) {
            *e->g_bbb220 = *(const uint32_t*)(e->arg + 0xc);
            e->f81E40();
            e->f88A70(0, 0, 0, 0);
        }
    } else if (*e->g_bbb214 == 2) {
        int32_t v = 0;
        *e->g_bbb214 = 1; *e->g_bbb220 = 0;
        if (e->arg) { e->f162D0(); v = e->edx_after_162D0; }
        e->f88A70(v, v, v, v);
    }
}
}
