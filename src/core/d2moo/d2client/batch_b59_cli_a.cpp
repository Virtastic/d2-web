// D2Client batch b59-cli-a: env-seam style; globals and callees come in through one struct (each member names its address).
#include <cstdint>
#include <emscripten.h>
struct D2cB59aEnv {
    int32_t* const* g_6fbcbf30;   // sound-entry list head (node: [0]=entry*, [1]=next)
    int32_t* g_6fbcbf9c; uint8_t* g_6fbc3b0b; int32_t* g_6fbcc2fc; int32_t* g_6fbc3a68;
    int32_t* g_6fbd3448; int32_t* g_6fbc3af0; uint16_t* g_6fbc3ad2;
    void (*f88230)(void* self, int32_t a, int32_t b, int32_t c);  // D2Client+0x88230 (thiscall)
    void (*f2b3d0)();                                             // D2Client+0x7B3D0
    void (*ord10043)(int32_t a, int32_t b);                       // free(0x68a,0)
    int32_t (*ord10048)();
    void (*fecc60)(int32_t a, const void* b);                     // FUN_6faecc60
    const void* g_6fb91388;
    void (*fac43e0)(const char* pkt);                             // FUN_6fac43e0
    void (*f2c9f0)();                                             // D2Client+0x7C9F0
};
extern "C" {
// 1.13c D2Client+0x7C9F0
EMSCRIPTEN_KEEPALIVE void d2c_7c9f0(const D2cB59aEnv* e) {
    int32_t* n = *e->g_6fbcbf30;
    if (*e->g_6fbcbf9c != 0) {
        for (; n; n = (int32_t*)n[1]) {
            int32_t* ent = (int32_t*)(intptr_t)*n;
            if (ent[1] == *e->g_6fbcbf9c) {
                void* self = (void*)(intptr_t)ent[2];
                if (self) e->f88230(self, (int32_t)(intptr_t)self, 0, 0);
                break;
            }
        }
        *e->g_6fbcbf9c = 0;
    }
    *e->g_6fbc3b0b = 0;
}

// 1.13c D2Client+0x7CEF0
EMSCRIPTEN_KEEPALIVE void d2c_7cef0(const D2cB59aEnv* e) {
    *e->g_6fbcc2fc = 0;
    if (*e->g_6fbc3a68 != 0) {
        e->f2b3d0();
        e->ord10043(0x68a, 0);
        *e->g_6fbc3a68 = 0;
    }
    e->f2c9f0();
    if (*e->g_6fbd3448 == 1) {
        *e->g_6fbd3448 = 0;
        e->fecc60(e->ord10048(), e->g_6fb91388);
    }
    if (*e->g_6fbc3af0 != 0) {
        char pkt[9] = {'1', -1, -1, -1, -1};
        uint32_t v = *e->g_6fbc3ad2;
        for (int i = 0; i < 4; i++) pkt[5 + i] = (char)(v >> (8 * i));  // uStack_7 overlaps bytes 5..8
        e->fac43e0(pkt);
        *e->g_6fbc3af0 = 0;
    }
}
}
