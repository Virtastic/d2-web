// D2Launch 1.13c +0xC430: replay probe over d2l_c430 (src/core/d2launch/d2launch_c430.cpp).
// Blobs: 0 lg hook log; 1 wz the object the Lang/gateway thunks return; 2..7 the output globals (eb8 eb0 edc ee0 ab4 aa8); 8 gate (DAT 25ad4);
// 9 cnt (25e78); 10 nul (256c4: the gateway pointer, 0 or the vt blob); 11 conn (25abc); 12 vt the gateway object; 13 cfg: +0 the Storm#422 result,
// +4/+8 the sub_BE50 results (first call, later calls).
#include <emscripten.h>
#include <cstdint>
#include "d2game_hooklog.h"

struct D2lC430S {
    void (*f15b60)();
    void (*dbg)(uint32_t text);
    void (*imp9aae)();
    void (*vcall10)(uint32_t gateway);
    uint32_t (*lang)(uint32_t ecx);
    void (*f15090)(uint32_t text, uint32_t cb, uint32_t one);
    void (*fe250)();
    uint32_t (*f12b10)(uint32_t idx);
    void (*imp99ac)(uint32_t h);
    void (*imp9904)(uint32_t h);
    void (*imp99be)(uint32_t h, uint32_t text);
    uint32_t (*imp9754)(uint32_t sect, uint32_t key, uint32_t def, uint32_t buf, uint32_t size);
    uint32_t (*fbe50)(uint32_t buf);
    uint32_t (*vcall98)(uint32_t gateway);
    uint32_t (*imp9b44)(uint32_t obj, uint32_t w3);
    void (*imp9790)(uint32_t dst, uint32_t src, uint32_t max);
    void (*imp9796)(uint32_t sect, uint32_t key, uint32_t def, uint32_t buf);
    uint32_t (*fb5b0)(uint32_t prev);
};
struct D2lC430G {
    uint32_t* g25eb8; int32_t g25ad4; int32_t g25e78; uint32_t g256c4; uint8_t* g25abc;
    uint32_t* g25eb0; uint32_t* g25edc; uint32_t* g25ee0; uint32_t* g25ab4; uint32_t* g25aa8;
};
extern "C" int32_t d2l_c430(const D2lC430G* g, uint32_t modbase, const D2lC430S* s);

static D2HookLog* g_lg;
static uint32_t* g_r;
static const uint32_t* g_cfg;
static uint32_t g_n12b, g_nbe;
static uint32_t wz() { return g_r[14 + 2 * 1]; }
static uint32_t tagWz(uint32_t v) { return v == wz() ? d2TagBlob(g_r, 1, (const void*)(uintptr_t)v) : v; }
static void f15b60() { g_lg->rec(0x15b60); }
static void dbg(uint32_t) {}
static void imp9aae() { g_lg->rec(0x9aae); }
static void vcall10(uint32_t gw) { g_lg->rec(0x9b32, 0, d2TagBlob(g_r, 12, (const void*)(uintptr_t)gw)); }
static uint32_t lang(uint32_t ecx) { g_lg->rec(0x9b32, 0, ecx); return wz(); }
static void f15090(uint32_t text, uint32_t cb, uint32_t one) { g_lg->rec(0x15090, 0, 0, 0, 0, 0, tagWz(text), cb, one); }
static void fe250() { g_lg->rec(0xe250); }
static uint32_t f12b10(uint32_t idx) { g_lg->rec(0x12b10, idx); return 0x1001 + g_n12b++; }
static void imp99ac(uint32_t h) { g_lg->rec(0x99ac, 0, h); }
static void imp9904(uint32_t h) { g_lg->rec(0x9904, 0, h); }
static void imp99be(uint32_t, uint32_t) { g_lg->rec(0x99be); }
static uint32_t imp9754(uint32_t a, uint32_t b, uint32_t c, uint32_t d, uint32_t) { g_lg->rec(0x9754, 0, 0, 0, 0, 0, a, b, c, d); return g_cfg[0]; }
static uint32_t fbe50(uint32_t) { g_lg->rec(0xbe50); return g_cfg[g_nbe++ ? 2 : 1]; }
static uint32_t vcall98(uint32_t) { g_lg->rec(0x9b38); return wz(); }
static uint32_t imp9b44(uint32_t obj, uint32_t w3) { g_lg->rec(0x9b44, 0, 0, 0, 0, 0, tagWz(obj), w3); return 0x5001; }
static void imp9790(uint32_t d, uint32_t sv, uint32_t n) { g_lg->rec(0x9790, 0, 0, 0, 0, 0, d, sv, n); }
static void imp9796(uint32_t a, uint32_t b, uint32_t c, uint32_t d) { g_lg->rec(0x9796, 0, 0, 0, 0, 0, a, b, c, d); }
static uint32_t fb5b0(uint32_t prev) { g_lg->rec(0xb5b0, prev); return 0x5002; }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_c430(uint32_t* r) {
    D2HookLog lg(r[14]);
    g_lg = &lg; g_r = r; g_n12b = g_nbe = 0;
    g_cfg = (const uint32_t*)(uintptr_t)r[14 + 2 * 13];
    auto blob = [&](int k) { return (uint32_t*)(uintptr_t)r[14 + 2 * k]; };
    D2lC430G G = { blob(2), (int32_t)*blob(8), (int32_t)*blob(9), *blob(10), (uint8_t*)blob(11),
                   blob(3), blob(4), blob(5), blob(6), blob(7) };
    static const D2lC430S S = { f15b60, dbg, imp9aae, vcall10, lang, f15090, fe250, f12b10, imp99ac, imp9904, imp99be,
                                imp9754, fbe50, vcall98, imp9b44, imp9790, imp9796, fb5b0 };
    r[4] = (uint32_t)d2l_c430(&G, 0xB2000000u, &S);
}
