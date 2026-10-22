// D2Client sub_7D200: panel/screen open init. Env-seam style; every global and callee comes in through one struct.
#include <cstdint>
#include <emscripten.h>
struct D2c7d200Env {
    int32_t *g_6fb91384, *g_6fbcec38, *g_6fbcb9a4, *g_6fbcb9a0, *g_6fb8bc48;
    int32_t *g_6fbc3af6, *g_6fba46c8, *g_6fba46c4, *g_6fbcc3dc, *g_6fbcc3e0, *g_6fbcc3f8, *g_6fbcc3fc, *g_6fbcc2f8, *g_6fbcc2f4;
    uint8_t* g_6fbc3aca;
    int16_t* g_6fbc3ad2;
    uint32_t** g_6fbc3a68;
    int32_t *g_6fbc3a7c, *g_6fbd3448, *g_6fbc3aec, *g_6fb8f0d8, *g_6fbcbbfc, *g_6fbcbf9c, *g_6fbc3af0;
    uint8_t *g_6fbc3a80, *g_6fbc3aba, *g_6fbc3b0b;
    int32_t* g_6fb8f398;                       // {id, key} pairs, 0-terminated
    const void* g_6fb91388; const void* g_6fb8359f;
    void (*f2c9f0)(); void (*f2ae00)();
    void (*ord10184)();
    int16_t* (*ord10003)(int32_t);
    uint32_t* (*f2c690)(int16_t*);
    int32_t (*ord10265)(int32_t);
    void (*ord10024)(const void*, int32_t);
    uint32_t (*getTickCount)();
    int32_t (*ord10048)();
    void (*feccc0)(int32_t, const void*);
    uint8_t (*f38a70)(int32_t, int32_t, int32_t, int32_t);
    void (*exit_)(int32_t);
};
extern "C" {
// 1.13c D2Client+0x7D200  (eax=flag, param_1=id, param_2)
EMSCRIPTEN_KEEPALIVE void d2c_7d200(const D2c7d200Env* e, int32_t flag, int16_t id, int32_t p2) {
    if (flag) { *e->g_6fb91384 = 0x105 - *e->g_6fbcb9a4; *e->g_6fbcec38 = *e->g_6fbcb9a0; }
    else { *e->g_6fbcec38 = (*e->g_6fb8bc48 - 0x145) / 2; *e->g_6fb91384 = 0xc; }
    *e->g_6fbc3af6 = flag != 0;
    *e->g_6fba46c8 = 0x10; *e->g_6fba46c4 = 0x10;
    *e->g_6fbcc3dc = 0; *e->g_6fbcc3e0 = 0; *e->g_6fbcc3f8 = 0; *e->g_6fbcc3fc = 0; *e->g_6fbcc2f8 = 0; *e->g_6fbcc2f4 = 0;
    e->f2c9f0();
    *e->g_6fbc3aca = 0;
    e->f2ae00();
    *e->g_6fbc3ad2 = id;
    if (*e->g_6fbc3a68) return;
    *e->g_6fbc3a7c = 0;
    e->ord10184();
    *e->g_6fbc3a68 = e->f2c690(e->ord10003(2));
    if (!*e->g_6fbc3a68) { e->ord10024(e->g_6fb8359f, e->ord10265(0x1051)); e->exit_(-1); return; }
    e->ord10184();
    bool first = *e->g_6fbd3448 == 0;
    *(int32_t*)((uint8_t*)*e->g_6fbc3a68 + 0x16) = 0;
    *e->g_6fbc3a80 = 1; *e->g_6fbc3aba = 1;
    if (first) {
        *e->g_6fbd3448 = 1;
        *e->g_6fbc3aec = (int32_t)e->getTickCount();
        e->feccc0(e->ord10048(), e->g_6fb91388);
    }
    if (*e->g_6fb8f0d8) {
        for (int32_t* p = e->g_6fb8f398; *p; p += 2) {
            if ((int16_t)p[1] == id) { *e->g_6fbc3b0b = e->f38a70(*e->g_6fbcbbfc, 5, 0, 0); *e->g_6fbcbf9c = *p; break; }
        }
    }
    if ((id == 0x7f || id == 0x4e22 || id == 0x4ec9 || id == 0x4ea3 || id == 0x18c) && p2) *e->g_6fbc3af0 = 1;
}
}
