// 1.13c D2Multi+0x10AA0 env form. No arguments. Walks the list headed by the first global (linked through +0xB0,
// handle at +0x9C) and the list headed by the second global (linked through +0x44), clears the counters, rebuilds the
// caption and calls 0x104D0 with zeros. The callees and the globals arrive in the env so the FUNCTION-mode probe drives them;
// production is d2_multi_sub_10aa0 in d2multi_batch_22.cpp.
#include <cstdint>

struct D2m10aa0Env {
    uint8_t** g_ef4;       // DAT_6fa09ef4 (list one head)
    uint8_t** g_ef8;       // DAT_6fa09ef8 (list two head)
    int32_t*  g_efc;       // DAT_6fa09efc
    int32_t*  g_f00;       // DAT_6fa09f00
    int32_t*  g_f04;       // DAT_6fa09f04
    void (*e860)(void);
    void (*ord10054)(int32_t, int32_t);
    void (*ord10043)(int32_t, int32_t);
    void (*fb40)(const char*);
    void (*sub104d0)(int32_t, int32_t);
};

extern "C" void d2multi_10aa0_env(const D2m10aa0Env* e) {
    e->e860();
    uint8_t* n = *e->g_ef4;
    while (n) {
        uint8_t* next = *(uint8_t**)(n + 0xb0);
        e->ord10054(*(int32_t*)(n + 0x9c), 1);
        *(int32_t*)(n + 0x9c) = 0;
        e->ord10043(0x814, 0);
        n = next;
    }
    *e->g_ef4 = nullptr;
    uint8_t* m = *e->g_ef8;
    while (m) {
        m = *(uint8_t**)(m + 0x44);
        e->ord10043(0x81c, 0);
    }
    *e->g_ef8 = nullptr;
    *e->g_efc = 0;
    *e->g_f00 = 0;
    e->fb40("caption (0)");
    *e->g_f04 = 0;
    e->sub104d0(0, 0);
}
