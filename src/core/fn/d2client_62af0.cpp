// d2client+0x62AF0 probe: r[0] = EBX, r[1] = st {c204, bbfc, d3460, d3464, d3468, d346c}, r[2] = hook log, r[3] = arr, r[5] = la, r[6] = lb, r[7] = count,
// r[8] = C1D2 return (x), r[9] = C1F6 return (y), r[10] = 62710 return, r[11] = C1F0 return (room), r[12] = CC3A sequence mode (0: la then lb forever; 1: la, lb, la forever).
// C1DE returns 0x77. Room pointers in arr point into the rm blob (blob 5) from arr[0].
#include <emscripten.h>
#include <cstdint>
typedef uint32_t u32; typedef int32_t i32;
struct D2c62af0G { u32 c204, bbfc, d3460, d3464, d3468, d346c; };
struct D2c62af0Env { u32 (*c1d2)(u32); u32 (*c1f6)(u32); u32 (*c1f0)(u32); u32 (*c1de)(u32); void (*c3dc)(u32, u32*, u32*); u32 (*cc3a)(u32); u32 (*f62710)(u32); void (*f62580)(u32, u32, u32, u32); void (*fatal)(); };
void d2c_62af0(D2c62af0G*, u32, const D2c62af0Env*);
namespace {
u32* g_lg; u32 g_arr, g_la, g_lb, g_cnt, g_x, g_y, g_ctx, g_room, g_mode, g_n, g_rm0;
void L(u32 rva, u32 a, u32 c, u32 s0, u32 s1, u32 s2, u32 s3) {
    u32* w = g_lg + 1 + g_lg[0]++ * 10; w[0] = rva; w[1] = a; w[2] = c; w[3] = 0; w[4] = 0; w[5] = 0; w[6] = s0; w[7] = s1; w[8] = s2; w[9] = s3;
}
u32 PR(u32 p) { return 0xB1050000u | (p - g_rm0); }
u32 c1d2(u32 a) { L(0xc1d2, 0, 0, a, 0, 0, 0); return g_x; }
u32 c1f6(u32 a) { L(0xc1f6, 0, 0, a, 0, 0, 0); return g_y; }
u32 c1f0(u32 a) { L(0xc1f0, 0, 0, a, 0, 0, 0); return g_room; }
u32 c1de(u32 a) { L(0xc1de, 0, 0, a >= g_rm0 && a < g_rm0 + 0x1000 ? PR(a) : a, 0, 0, 0); return 0x77; }
void c3dc(u32 room, u32* arr, u32* cnt) { L(0xc3dc, 0, 0, room, 0xB3000000u, 0xB3000000u, 0); *arr = g_arr; *cnt = g_cnt; }
u32 cc3a(u32 c) { L(0xcc3a, 0, c, 0, 0, 0, 0); u32 n = g_n++; bool a = n == 0 || (g_mode == 1 && n >= 2); return a ? g_la : g_lb; }
u32 f62710(u32 a) { L(0x62710, a, 0, 0, 0, 0, 0); return g_ctx; }
void f62580(u32 room, u32 z, u32 ctx, u32 ebx) { L(0x62580, 0, 0, PR(room), z, ctx, ebx); }
void fatal() {}
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_62af0(u32* r) {
    g_lg = (u32*)(uintptr_t)r[2]; g_lg[0] = 0; g_arr = r[3]; g_la = r[5]; g_lb = r[6]; g_cnt = r[7]; g_x = r[8]; g_y = r[9]; g_ctx = r[10]; g_room = r[11]; g_mode = r[12]; g_n = 0;
    g_rm0 = ((const u32*)(uintptr_t)g_arr)[0];
    D2c62af0Env e{c1d2, c1f6, c1f0, c1de, c3dc, cc3a, f62710, f62580, fatal};
    d2c_62af0((D2c62af0G*)(uintptr_t)r[1], r[0], &e);
    r[4] = 0;
}
