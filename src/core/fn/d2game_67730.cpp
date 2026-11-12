// 1.13c D2Game+0x67730 probe: the port over the HookEnv recorder.
// Descriptor `args 2` (stdcall, no regs): r[0] game (= quest record + 0x74), r[1] player. Blobs g lst n0 nd jk pl pl2 cain plr cbe lg sc: cain r[28],
// cbe r[32] (global 0x111C20), hook log r[34], hook values r[36].
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"

struct D2Game67730Env {
    void     (*fatal)(uint32_t line);
    void     (*a4a2)(uint32_t unit, uint32_t v);
    int32_t  (*sub1100)(uint32_t unit);
    int32_t  (*sub1130)(uint32_t unit);
    uint32_t (*a478)(uint32_t unit);
    uint32_t (*subEF650)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6);
    void     (*sub34130)(uint32_t room, uint32_t pt, uint32_t a3, uint32_t a4, uint32_t outRoom, uint32_t a6);
    uint32_t globalCBE70;
    void     (*subCBE70)(uint32_t edx, uint32_t s1, uint32_t s2);
    uint32_t strCBE70;
    uint32_t (*subDFCC0)(uint32_t eax, uint32_t ebx, uint32_t edi, uint32_t s1);
    void     (*subDFFD0)(uint32_t esi, uint32_t s1, uint32_t s2);
    uint32_t (*a808)(uint32_t room);
    uint8_t  (*a56e)(uint32_t level);
    uint32_t (*a556)(uint32_t room);
    uint32_t (*ae98)(uint32_t room, uint32_t pt, uint32_t c, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7);
    uint32_t (*sub1280)(uint32_t room, uint32_t x, uint32_t y);
    uint32_t (*subE1D90)(uint32_t ecx, uint32_t edx, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7);
    uint32_t (*subA22E0)(uint32_t game, uint32_t obj, uint32_t one, uint32_t level);
    uint8_t  (*aee0)(uint32_t obj);
    void     (*aec8)(uint32_t obj, uint32_t v);
    void     (*a51a)(uint32_t room, uint32_t v);
};
uint32_t D2Game_67730(uint32_t game, uint32_t player, const D2Game67730Env* env);

namespace {
HookEnv* H;
uint32_t gCain;   // blob 7: what the retail 0xEF650 hook hands back (a nonzero hook value)
inline uint32_t P(const void* p) { return (uint32_t)(uintptr_t)p; }
inline uint32_t M(uint32_t va) { return 0xB2000000u | (va - 0x6fc20000u); }   // a module address, as the capture's scrub records it
void fatal(uint32_t) { abort(); }
void a4a2(uint32_t u, uint32_t v) { H->h(0xA4A2, 0, 0, 0, 0, 0, u, v); }
int32_t s1100(uint32_t u) { return (int32_t)H->h(0x1100, 0, u); }
int32_t s1130(uint32_t u) { return (int32_t)H->h(0x1130, 0, u); }
uint32_t a478(uint32_t u) { return H->h(0xA478, 0, 0, 0, 0, 0, u); }
uint32_t ef650(uint32_t c, uint32_t d, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6) {
    return H->h(0xEF650, 0, c, d, 0, 0, s1, s2, s3, s4, s5, s6) ? gCain : 0;
}
void s34130(uint32_t room, uint32_t pt, uint32_t a3, uint32_t a4, uint32_t out, uint32_t a6) {
    // the capture logs the block as the callee received it, before the hook writes its answer through the same pointer
    const uint32_t in[2] = { HookEnv::rd32(pt), HookEnv::rd32(pt + 4) };
    H->h(0x34130, 0, 0, 0, 0, 0, room, pt, a3, a4, out, a6); H->blk(0x34130, in, 2);
}
void sebe70(uint32_t edx, uint32_t s1, uint32_t s2) { H->h(0xCBE70, 0, 0, M(edx), 0, 0, s1, s2); }
uint32_t sdfcc0(uint32_t eax, uint32_t ebx, uint32_t edi, uint32_t s1) { H->ed(edi); const uint32_t v = H->h(0xDFCC0, eax, 0, 0, M(ebx), 0, s1); H->edi(0xDFCC0, edi); return v; }
void sdffd0(uint32_t esi, uint32_t s1, uint32_t s2) { H->h(0xDFFD0, 0, 0, 0, 0, esi, s1, s2); }
uint32_t a808(uint32_t r) { return H->h(0xA808, 0, 0, 0, 0, 0, r); }
uint8_t a56e(uint32_t v) { return (uint8_t)H->h(0xA56E, 0, 0, 0, 0, 0, v); }
uint32_t a556(uint32_t r) { return H->h(0xA556, 0, 0, 0, 0, 0, r); }
uint32_t ae98(uint32_t room, uint32_t pt, uint32_t c, uint32_t a4, uint32_t a5, uint32_t a6, uint32_t a7) { return H->h(0xAE98, 0, 0, 0, 0, 0, room, pt, c, a4, a5, a6, a7); }
uint32_t s1280(uint32_t room, uint32_t x, uint32_t y) { H->ed(y); uint32_t v = H->h(0x1280, room, 0, 0, x); H->edi(0x1280, y); return v; }
uint32_t e1d90(uint32_t c, uint32_t d, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7) { return H->h(0xE1D90, 0, c, d, 0, 0, s1, s2, s3, s4, s5, s6, s7); }
uint32_t a22e0(uint32_t g, uint32_t o, uint32_t one, uint32_t l) { return H->h(0xA22E0, 0, 0, 0, 0, 0, g, o, one, l); }
uint8_t aee0(uint32_t o) { return (uint8_t)H->h(0xAEE0, 0, 0, 0, 0, 0, o); }
void aec8(uint32_t o, uint32_t v) { H->h(0xAEC8, 0, 0, 0, 0, 0, o, v); }
void a51a(uint32_t r, uint32_t v) { H->h(0xA51A, 0, 0, 0, 0, 0, r, v); }
}

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_67730(uint32_t* r) {
    HookEnv e((uint32_t*)(uintptr_t)r[34], (const uint32_t*)(uintptr_t)r[36]);
    H = &e; gCain = r[28];
    const D2Game67730Env env = { fatal, a4a2, s1100, s1130, a478, ef650, s34130, HookEnv::rd32(r[32]), sebe70, 0x6fd1beacu, sdfcc0, sdffd0, a808, a56e, a556, ae98,
                                 s1280, e1d90, a22e0, aee0, aec8, a51a };
    r[4] = D2Game_67730(r[0], r[1], &env);
}
