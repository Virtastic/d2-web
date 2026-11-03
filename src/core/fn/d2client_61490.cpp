// d2client+0x61490 probe: r[0] = unit (EAX), r[1] = out byte (EBX), r[2] = st {bbfc, f16b8, col[12] = C210..}, r[3] = hook log, r[5] = hv.
// hv: 0 C1F0, 1 C1DE, 2 f1760, 3 C658 kind written back, 4 CBDA, 5 560D0, 6 55B60 first, 7 55B60 second, 8 219F0, 9 CC52. bbfc = the u2 blob (blob 1), unit = blob 0 or u2.
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32; typedef int32_t i32;
struct D2c61490Env { u32 (*c1f0)(u32); u32 (*c1de)(u32); u32 (*f1760)(u32, u32); void (*c658)(u32, i32*, i32*, i32*, u32, u32); u32 (*cbda)(u32, u32); u32 (*f560d0)(u32); u32 (*f55b60)(u32); u32 (*f219f0)(u32); u32 (*cc52)(u32); };
u32 d2c_61490(u32, uint8_t*, u32, u32, const uint8_t*, const D2c61490Env*);
namespace {
u32* g_lg; const u32* g_hv; u32 g_u2, g_n55;
u32 P(u32 p) { return !p ? 0 : (p == g_u2 ? 0xB1010000u : 0xB1000000u); }
void L(u32 rva, u32 a, u32 c, u32 d, u32 s0, u32 s1, u32 s2, u32 s3) {
    u32* w = g_lg + 1 + g_lg[0]++ * 10; w[0] = rva; w[1] = a; w[2] = c; w[3] = d; w[4] = 0; w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
}
u32 c1f0(u32 a) { L(0xc1f0, 0, 0, 0, P(a), 0, 0, 0); return g_hv[0]; }
u32 c1de(u32 a) { L(0xc1de, 0, 0, 0, a, 0, 0, 0); return g_hv[1]; }
u32 f1760(u32 t, u32 g) { L(0x1760, t, 0, g, 0, 0, 0, 0); return g_hv[2]; }
void c658(u32 u, i32* k, i32*, i32*, u32, u32) { L(0xc658, 0, 0, 0, P(u), 0xB3000000u, 0xB3000000u, 0xB3000000u); *k = (i32)g_hv[3]; }
u32 cbda(u32 u, u32 w) { L(0xcbda, 0, 0, 0, P(u), w, 0, 0); return g_hv[4]; }
u32 f560d0(u32 u) { L(0x560d0, P(u), 0, 0, 0, 0, 0, 0); return g_hv[5]; }
u32 f55b60(u32 u) { L(0x55b60, P(u), 0, 0, 0, 0, 0, 0); return g_hv[6 + (g_n55++ ? 1 : 0)]; }
u32 f219f0(u32 v) { L(0x219f0, v, 0, 0, 0, 0, 0, 0); return g_hv[8]; }
u32 cc52(u32 p) { L(0xcc52, 0, 0, 0, p, 0, 0, 0); return g_hv[9]; }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_61490(u32* r) {
    g_lg = (u32*)(uintptr_t)r[3]; g_lg[0] = 0; g_hv = (const u32*)(uintptr_t)r[5]; g_n55 = 0;
    const u32* st = (const u32*)(uintptr_t)r[2]; g_u2 = st[0];
    D2c61490Env e{c1f0, c1de, f1760, c658, cbda, f560d0, f55b60, f219f0, cc52};
    r[4] = d2c_61490(r[0], (uint8_t*)(uintptr_t)r[1], st[0], st[1], (const uint8_t*)(st + 2), &e);
}
