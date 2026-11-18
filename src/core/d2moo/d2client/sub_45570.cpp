// D2Client+0x45570: env-seam style; EBX (unaff) passed as `ebx`.
#include <cstdint>
#include <emscripten.h>
struct D2c45570Env {
    int32_t* g_6fbcc348; int32_t* g_6fbcc24c; int32_t* g_6fbcc250; int32_t* g_6fbcc25c;
    int32_t* g_6fbcc268; int32_t* g_6fbcc350; int32_t* g_6fbcc354; int32_t* g_6fbcc358;
    int32_t* g_6fbcc35c; int32_t* g_6fbcc360; int32_t* g_6fbcc364; int32_t* g_6fbcc368;
    int32_t* g_6fbcc36c; int32_t* g_6fbcc370; int32_t* g_6fbc97f8; int32_t* g_6fbc75b8;
    int32_t* const* g_6fbcbf30;            // list head (node: [0]=entry*, [1]=next)
    bool (*f88080)(int32_t);               // D2Client+0x88080
    void (*f888B0)();                      // D2Client+0x888B0
};
extern "C" {
// 1.13c D2Client+0x45570
EMSCRIPTEN_KEEPALIVE void d2c_45570(const D2c45570Env* e, int32_t p1, int32_t p2, int32_t p3,
                                    int32_t p4, int32_t p5, int32_t p6, int32_t ebx) {
    int32_t idx = *e->g_6fbcc348;
    if (idx != 0) {
        int32_t base = (*e->g_6fbcc24c == 0 || idx < 0 || *e->g_6fbcc250 <= idx)
                           ? 0 : idx * 0x8e + *e->g_6fbcc24c;
        for (int32_t* n = *e->g_6fbcbf30; n; n = (int32_t*)n[1]) {
            int32_t* ent = (int32_t*)*n;
            if (ent[1] == idx) {
                int32_t u = ent[2];
                if (u != 0 && e->f88080(u)) {
                    int32_t v = ebx ? *(int32_t*)(intptr_t)(base + 0x50 + ebx * 4) : 0;
                    ((int32_t*)(intptr_t)*e->g_6fbc75b8)[idx - *e->g_6fbcc25c] = v;
                    *e->g_6fbcc350 = *e->g_6fbcc268;
                }
                break;
            }
        }
    }
    e->f888B0();
    *e->g_6fbcc358 = p1; *e->g_6fbcc354 = 1; *e->g_6fbcc360 = 1;
    *e->g_6fbcc35c = p2 + *e->g_6fbc97f8; *e->g_6fbcc364 = p4;
    *e->g_6fbcc368 = *e->g_6fbc97f8 + p5; *e->g_6fbcc36c = p6;
    *e->g_6fbcc370 = *e->g_6fbc97f8 + p3;
}
}
