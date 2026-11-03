// d2client+0x60AF0 probe: r[0] = unit (EDI), r[1] = st {bbfc, div, flag, orgX, orgY, col, clip[4], seg[26]}, r[2] = hook log, r[3] = C2F8 x, r[5] = C2F8 y, r[6] = C1EA first, r[7] = C1EA second.
// Hooks: C1F0 returns 0x51, C1DE 0x52, D280 0x1234 (cdecl, leaves x, y, 2 for 601A0); 601A0 logs EBX and 4 stack words.
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32; typedef int32_t i32;
struct D2c60af0Env { u32 (*c1f0)(u32); u32 (*c1de)(u32); u32 (*c1ea)(u32); void (*c2f8)(i32*, i32*); void (*d102)(i32, i32, i32, i32, u32, u32); u32 (*d280)(u32, i32, i32, u32); void (*f601a0)(u32); };
void d2c_60af0(u32, u32, i32, i32, i32, const i32*, uint8_t, const i32*, u32, const D2c60af0Env*);
namespace {
u32* g_lg; i32 g_x, g_y; u32 g_ea[2]; u32 g_n;
void L(u32 rva, u32 a, u32 b, u32 s0, u32 s1, u32 s2, u32 s3) {
    u32* w = g_lg + 1 + g_lg[0]++ * 10; w[0] = rva; w[1] = a; w[2] = 0; w[3] = 0; w[4] = b; w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
}
u32 c1f0(u32 a) { L(0xc1f0, 0, 0, a, 0, 0, 0); return 0x51; }
u32 c1de(u32 a) { L(0xc1de, 0, 0, a, 0, 0, 0); return 0x52; }
u32 c1ea(u32 a) { L(0xc1ea, 0, 0, a, 0, 0, 0); return g_ea[g_n++ & 1]; }
void c2f8(i32* x, i32* y) { L(0xc2f8, 0, 0, 0xB3000000u, 0xB3000000u, 0, 0); *x = g_x; *y = g_y; }
void d102(i32 a, i32 b, i32 c, i32 d, u32, u32) { L(0xd102, 0, 0, (u32)a, (u32)b, (u32)c, (u32)d); }
u32 d280(u32 u, i32 x, i32 y, u32 m) { L(0x9d280, u ? 0xB1000000u : 0, 0, (u32)x, (u32)y, m, 0); return 0x1234; }
void f601a0(u32 ebx) { L(0x601a0, 0, ebx, 0, 0, 0, 0); }
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_60af0(u32* r) {
    g_lg = (u32*)(uintptr_t)r[2]; g_lg[0] = 0; g_x = (i32)r[3]; g_y = (i32)r[5]; g_ea[0] = r[6]; g_ea[1] = r[7]; g_n = 0;
    const i32* st = (const i32*)(uintptr_t)r[1];
    D2c60af0Env e{c1f0, c1de, c1ea, c2f8, d102, d280, f601a0};
    d2c_60af0(r[0], (u32)st[0], st[1], st[3], st[4], st + 6, (uint8_t)st[5], st + 10, (u32)st[2], &e);
    r[4] = 0;
}
