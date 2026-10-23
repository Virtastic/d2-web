// D2Client sub_C30E0 (size 1095): create the 0xd7 x 0x8c popup window + 4 child objects for mode 0..4 (2/3/6/7 extras).
// Decompile-derived; callees go through D2cHost in retail's call order. Ordinal seams use rva = 0x100000 + ordinal (UNVERIFIED names).
// Plain callees use RVA = addr - 0x6FAB0000 (image base 0x6FAB0000 in this build); 0x200000+off = vtable slot seam.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
typedef uint32_t u32; typedef int32_t i32;
struct D2cC30E0G {  // globals, all 32-bit retail
    u32 *g_cbbfc /*6fbcbbfc*/, *g_ad60 /*6fbaad60 window*/, *g_ad74 /*6fbaad74*/, *g_cc434 /*6fbcc434*/,
        *g_d349c /*6fbd349c*/, *g_d3498 /*6fbd3498*/, *g_cbbb0 /*6fbcbbb0*/, *g_cbbbc /*6fbcbbbc*/, *g_ad5c /*6fbaad5c*/,
        *g_ad64, *g_ad68, *g_ad6c, *g_ad70;  // 6fbaad64..70 child objects
};
namespace {
inline u32 go(D2cHost h, u32 rva, u32 ecx = 0, u32 edx = 0, u32 s0 = 0, u32 s1 = 0, u32 s2 = 0, u32 s3 = 0) {
    D2cCall c = {}; c.rva = rva; c.ecx = ecx; c.edx = edx; c.s[0] = s0; c.s[1] = s1; c.s[2] = s2; c.s[3] = s3; return h(&c);
}
inline u32 O(u32 n) { return 0x100000 + n; }
inline u32 vcall(D2cHost h, u32 obj, u32 off, u32 a = 0) { return go(h, 0x200000 + off, obj, a); }  // *(*obj+off)(obj,a) seam
}  // namespace
extern "C" {
// 1.13c D2Client+0xC30E0  (param_1 = mode, fastcall/stack as retail)
EMSCRIPTEN_KEEPALIVE void d2c_c30e0(D2cHost h, D2cC30E0G* g, i32 mode) {
    if (*g->g_cbbfc == 0 || go(h, O(11017)) != 0) { go(h, 0xBF720); return; }
    if (*g->g_ad60 != 0) return;
    if (*g->g_ad74 == 0) {
        *g->g_cc434 = 0;
        if (*g->g_d349c == 1) {
            *g->g_d349c = 0;
            go(h, 0x3CC60, go(h, O(10048)), 0xFBA51BC);
            if (*g->g_d3498 == 0) { go(h, 0x3CCC0, go(h, O(10048)), 0xFBA51EC); *g->g_d3498 = 1; }
        }
        *g->g_ad74 = 1;
    }
    *g->g_cbbb0 = 0;
    u32 u;
    switch (mode) {
    case 0: u = 0xe; *g->g_cbbbc = 1; break;
    case 1: case 2: case 3: go(h, 0x987F0); u = 0xe; break;  // FUN_6fb487f0
    case 4: go(h, 0x987F0); u = 0xf; break;
    default: go(h, 0xBF720); return;
    }
    go(h, O(10973), *g->g_cbbfc, u);
    *g->g_ad5c = (u32)mode;
    *g->g_ad60 = go(h, 0xA4040, 0xd7, 0x8c, 0x6fb6feb0, 0, 0);  // FUN_6fb54040 create window
    go(h, 0xA3340, 1);
    go(h, O(10184));
    // 0x100 Unicode default-ctors + 0x200-byte zero, then title string (mode==5 never reached: switch rejects it)
    go(h, O(10003));  // title text -> Unicode::strcpy into local buf
    for (u32 p = go(h, O(10059), 200); p; p = go(h, 0x200000 + 4, p)) {  // list walk: node[0]=text, node[1]=next
        go(h, 0xA36F0, *g->g_ad60, p, 0x18, 0, 1, 0);
        go(h, O(10043), 0x182b); go(h, O(10043), 0x182c);
    }
    *g->g_ad64 = go(h, 0x3F500, go(h, O(401), 0xc, 0xFB804F0, 0xffffffff, 0));
    go(h, 0xA3400);
    *g->g_ad68 = go(h, 0x38A20, go(h, O(401), 0xc, 0xFB804F0, 0xffffffff, 0), 0x102, 0xe4, 100);
    go(h, 0xA3400);
    if (*g->g_ad68) vcall(h, *g->g_ad68, 0x44, 1);
    *g->g_ad6c = go(h, 0x66EB0, go(h, O(401), 0xc, 0xFB804F0, 0xffffffff, 0));
    go(h, 0xA3400);
    vcall(h, *g->g_ad6c, 0x30, 1);
    *g->g_ad70 = go(h, 0x66EB0, go(h, O(401), 0xc, 0xFB804F0, 0xffffffff, 0));
    go(h, 0xA3400);
    if (mode == 2) { go(h, 0x595C0); return; }  // FUN_6fb095c0
    if ((mode == 3 || mode == 7 || mode == 6) && *g->g_ad68) vcall(h, *g->g_ad68, 0x38);
}
}
