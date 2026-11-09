// d2game+0x89C30 probe: r[0] = ECX game, r[1] = EDX unit, r[2] = stack task. Blobs lg 0, sc 1, game 2, unit 3, ud 4, ai 5, msub 6, usub 7, task 8, M 9,
// row 10, tgt 11, tsub 12, foe 13. Callees beyond the shared D77 table are bound here with the registers the descriptor's hooks log; the 0xB17B0
// scan struct is logged as it stands at entry (dword 3 is stack residue, masked), and the same stack local is the 0xA694 pointer (logged 0xB3000000).
#include "d2game_77d_probe.h"
extern "C" void d2game_89c30_port(const D2Game77dEnv* env, uint32_t game, uint32_t unit, uint32_t task);
namespace {
using D77::E;
void b17b0(uint32_t m, uint32_t buf, uint32_t g, uint32_t u, uint32_t cb) {
    uint32_t pre[4]; for (uint32_t i = 0; i < 4; i++) pre[i] = HookEnv::rd32(buf + 4 * i);
    E->h(0xB17B0, m, buf, g, 0, 0, u, 0xB2000000u | cb);
    E->blk(0xB17B0, pre, 4, 0x7);
}
void f1d90(uint32_t mode, uint32_t ecx, uint32_t g, uint32_t u, uint32_t a, uint32_t b) { E->h(0xF1D90, mode, ecx, 0, 0, 0, g, u, a, b); }
void f20c0(uint32_t g, uint32_t u, uint32_t v) { E->h(0xF20C0, g, 0, 0, u, 0, v); }
void f1ff0(uint32_t m, uint32_t u, uint32_t g, uint32_t x, uint32_t y) { E->h(0xF1FF0, m, u, 0, 0, 0, g, x, y); }
uint32_t b07b0(uint32_t u, uint32_t x, uint32_t y) { return E->h(0xB07B0, u, 0, 0, 0, 0, x, y); }
uint32_t s89730(uint32_t u, uint32_t g) { return E->h(0x89730, u, 0, 0, 0, 0, g); }
void s7be10(uint32_t g, uint32_t u) { E->h(0x7BE10, g, 0, 0, 0, u); }
uint32_t s1450(uint32_t seed, uint32_t n) { return E->h(0x1450, 0, 0, 0, 0, seed, n); }
uint32_t s11d0(uint32_t seed, uint32_t n) { return E->h(0x11D0, 0, seed, 0, 0, n); }
uint32_t a71e(uint32_t u, uint32_t m, uint32_t one) { return E->h(0xA71E, 0, 0, 0, 0, 0, u, m, one); }
uint32_t a694(uint32_t room, uint32_t ptr, uint32_t mask) { return E->h(0xA694, 0, 0, 0, 0, 0, room, ptr, mask); }
uint32_t e03a0(uint32_t a, uint32_t c, uint32_t d) { return E->h(0xE03A0, a, c, d); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_89c30(uint32_t* r) {
    uint32_t* lg = (uint32_t*)(uintptr_t)r[14];
    HookEnv e(lg, (const uint32_t*)(uintptr_t)r[16]); D77::E = &e;
    D2Game77dEnv v = D77::env();
    v.subB17B0m = b17b0; v.subF1D90 = f1d90; v.subF20C0 = f20c0; v.subF1FF0 = f1ff0; v.subB07B0 = b07b0; v.sub89730 = s89730;
    v.sub7BE10 = s7be10; v.sub1450 = s1450; v.sub11D0 = s11d0; v.th2A71E = a71e; v.th2A694 = a694; v.subE03A0raw = e03a0;
    d2game_89c30_port(&v, r[0], r[1], r[2]);
    r[4] = 0;
    d2TagLog(lg, r, 14);
}
