// 1.13c D2Multi+0x104D0 env form. fastcall: ECX unused, EDX = delta. The globals and
// callees arrive in the env so the FUNCTION-mode probe drives them; production is d2_multi_sub_104d0 in d2multi_batch_22.cpp.
#include <cstdint>

struct D2m104d0Env {
    int32_t* g_f00;        // DAT_6fa09f00 (count)
    int32_t* g_f04;        // DAT_6fa09f04 (scroll position)
    void (*sub_f750)(void);
    void (*ord10147)(void);
};

extern "C" void d2multi_104d0_env(const D2m104d0Env* e, int32_t delta) {
    int32_t old = *e->g_f04;
    if (*e->g_f00 < 0xc) {
        *e->g_f04 = 0;
    } else {
        int32_t s = *e->g_f04 + delta;
        *e->g_f04 = s < 1 ? 0 : s;
        if (*e->g_f00 - 0xb <= *e->g_f04) *e->g_f04 = *e->g_f00 - 0xb;
    }
    if (*e->g_f04 != old) e->sub_f750();
    e->ord10147(); e->ord10147(); e->ord10147(); e->ord10147();
}
