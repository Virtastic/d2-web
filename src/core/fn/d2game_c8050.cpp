// d2game+0xC8050 probe: r[0] = ECX game, r[1] = EDX event record. Blobs: lg 0, sc 1, game 2, rec 3, own 4, tab 5, st 6, g4 7, lst 8, n1 9, n2 10, un 11,
// dbg 12 (gvar 0x111C20, the log flag). Every callee is a HookEnv recorder logging what the descriptor's hook line names.
// Code pointers the port writes or passes are 0xB2000000|rva (the capture's module-relative tag). The fatal seam aborts: no tuple reaches it.
#include <emscripten.h>
#include <cstdint>
#include "hookenv.h"
#include "d2game_probe_tag.h"
#include "../d2game/d2game_c8050.h"
namespace {
HookEnv* E; uint32_t DBG;
uint32_t a4c0(uint32_t u) { return E->h(0xA4C0, 0, 0, 0, 0, 0, u); }
uint32_t f1130(uint32_t c) { return E->h(0x1130, 0, c); }
uint32_t f1100(uint32_t c) { return E->h(0x1100, 0, c); }
uint32_t a478(uint32_t u) { return E->h(0xA478, 0, 0, 0, 0, 0, u); }
uint32_t a808(uint32_t v) { return E->h(0xA808, 0, 0, 0, 0, 0, v); }
void dffd0(uint32_t esi, uint32_t s0, uint32_t s1) { E->h(0xDFFD0, 0, 0, 0, 0, esi, s0, s1); }
uint32_t a56e(uint32_t v) { return E->h(0xA56E, 0, 0, 0, 0, 0, v); }
uint32_t a556(uint32_t v) { return E->h(0xA556, 0, 0, 0, 0, 0, v); }
uint32_t ae98(uint32_t u, uint32_t* xy, uint32_t k, uint32_t f1, uint32_t f2, uint32_t z) {
    const uint32_t p = E->stk(xy);
    const uint32_t v = E->h(0xAE98, 0, 0, 0, 0, 0, u, p, p, k, f1, f2, z);
    const uint32_t w[3] = { f1, f2, z }; E->x(0xAE98, w, 3);
    return v;
}
uint32_t f1280(uint32_t a) { return E->h(0x1280, a); }
uint32_t e1d90(uint32_t c, uint32_t d, const uint32_t* w) {
    const uint32_t v = E->h(0xE1D90, 0, c, d, 0, 0, w[0], w[1], w[2], w[3], w[4], w[5], w[6]);
    E->x(0xE1D90, w + 4, 3);
    return v;
}
void a4a2(uint32_t u, uint32_t z) { E->h(0xA4A2, 0, 0, 0, 0, 0, u, z); }
uint32_t a22e0(const uint32_t* w) { return E->h(0xA22E0, 0, 0, 0, 0, 0, w[0], w[1], w[2], w[3]); }
uint32_t aee0(uint32_t u) { return E->h(0xAEE0, 0, 0, 0, 0, 0, u); }
void aec8(uint32_t u, uint32_t v) { E->h(0xAEC8, 0, 0, 0, 0, 0, u, v); }
void a51a(uint32_t a, uint32_t b) { E->h(0xA51A, 0, 0, 0, 0, 0, a, b); }
void f35eb0(uint32_t eax, uint32_t esi) { E->h(0x35EB0, eax, 0, 0, 0, esi); }
void dfcc0(uint32_t ebx, uint32_t edi, uint32_t stk) { E->h(0xDFCC0, 0, 0, 0, ebx, 0, stk); E->edi(0xDFCC0, edi); }
void f35910(uint32_t eax, uint32_t ecx, uint32_t stk) { E->h(0x35910, eax, ecx, 0, 0, 0, stk); }
void cbe70(uint32_t edx, uint32_t g4, uint32_t four) { E->h(0xCBE70, 0, 0, edx, 0, 0, g4, four); }
void f366a0(uint32_t eax, uint32_t ecx, uint32_t esi) { E->h(0x366A0, eax, ecx, 0, 0, esi); }
void f33920(uint32_t eax, uint32_t a, uint32_t b) { E->h(0x33920, eax, 0, 0, 0, 0, a, b); }
void f34c00(uint32_t eax, uint32_t g4) { E->h(0x34C00, eax, 0, 0, 0, 0, g4); }
void f33880(uint32_t eax, uint32_t esi, uint32_t ebp, uint32_t edi) { E->h(0x33880, eax, 0, 0, 0, esi); E->y(0x33880, ebp, edi); }
uint32_t a568(uint32_t u, uint32_t a, uint32_t b) { return E->h(0xA568, 0, 0, 0, 0, 0, u, a, b); }
void a57a(uint32_t u, uint32_t a, uint32_t b) { E->h(0xA57A, 0, 0, 0, 0, 0, u, a, b); }
void a586(uint32_t u, uint32_t a, uint32_t b) { E->h(0xA586, 0, 0, 0, 0, 0, u, a, b); }
void nodeCall(uint32_t node, uint32_t fn) { E->h(HookEnv::handlerRva(fn), 0, node); }
uint32_t logOn() { return HookEnv::rd32(DBG); }
uint32_t isBad(uint32_t) { return 0; }   // iat IsBadCodePtr 1 0
void fatal(uint32_t) { abort(); }
uint32_t fp(uint32_t rva) { return 0xB2000000u | rva; }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_c8050(uint32_t* r) {
    uint32_t* lg = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(lg, (const uint32_t*)(uintptr_t)r[16]); E = &e; DBG = r[14 + 2 * 12];
    const GC8050Env env{ a4c0, f1130, f1100, a478, a808, dffd0, a56e, a556, ae98, f1280, e1d90, a4a2, a22e0, aee0, aec8, a51a, f35eb0, dfcc0, f35910, cbe70, f366a0,
                         f33920, f34c00, f33880, a568, a57a, a586, nodeCall, logOn, isBad, fatal, fp };
    d2game_c8050_port(&env, r[0], r[1]);
    r[4] = 0;
    static const uint32_t sz[13] = { 0x964, 0x98, 0x100, 0x20, 0x80, 0x40, 0x40, 0x1100, 0x10, 0x100, 0x100, 0x20, 4 };
    d2TagLogSz(lg, r, 13, sz);
}
