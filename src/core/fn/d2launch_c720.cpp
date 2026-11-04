// D2Launch 1.13c +0xC720: replay probe over d2l_c720 (src/core/d2launch/d2launch_c720.cpp).
// Args: r[0] = EAX description, r[1] = a0 text, r[2] = wire record, r[3] = a2, r[5] = a3, r[6] = a4.
// Blobs: 0 lg hook log; 1 rec; 2 str (a0); 3 desc; 4 node (what the allocator hands out); 5 head (DAT 25EC8); 6 cnt (25ED0); 7 flag (25EB0);
// 8..10 l1..l3 the existing list nodes; 11 cfg: +0 the 0x9F40 result, +4..+12 the three 0x9856 results, +16/+20 the 0x980E results (first call, later calls),
// +24/+28 the 0x99CA results (first call, later calls).
#include <emscripten.h>
#include <emscripten/stack.h>
#include <cstdint>
#include <cstring>
#include "d2game_hooklog.h"

struct D2lC720S {
    uint32_t (*alloc)(uint32_t size, uint32_t file, uint32_t line);
    void (*copy)(uint32_t dst, uint32_t src, uint32_t max);
    void (*w2u)(uint32_t dst, uint32_t src, uint32_t max);
    void (*u2w)(uint32_t dst, uint32_t src, uint32_t max);
    uint32_t (*rd16)(uint32_t p);
    uint32_t (*f9f40)(uint32_t name);
    void (*f3a0)(uint32_t buf, uint32_t node);
    void (*cpy3)(uint32_t dst, uint32_t src, uint32_t n);
    uint32_t (*f980e)();
    uint32_t (*mk)(uint32_t cls, uint32_t type, uint32_t p300, uint32_t p310);
    void (*fail)(uint32_t id);
    void (*f9a0c)(uint32_t h, uint32_t z);
    void (*f9a06)(uint32_t h, uint32_t z);
    void (*f9a2a)(uint32_t h, uint32_t five);
    void (*fb600)();
};
struct D2lC720G { uint32_t* head; int32_t* count; uint32_t* flag; };
extern "C" int32_t d2l_c720(uint32_t desc, uint32_t a0, uint32_t rec, uint32_t a2, uint32_t a3, uint32_t a4,
                            const D2lC720G* g, uint32_t modbase, const D2lC720S* s);

static D2HookLog* g_lg;
static uint32_t* g_r;
static const uint32_t* g_cfg;
static uint32_t g_n16, g_n80e, g_n99ca;
// a pointer as the capture records it: a blob address 0xB1<blob><off>, a stack address 0xB3000000, anything else as is
static uint32_t tagp(uint32_t p) {
    static const uint32_t sz[12] = { 0, 64, 64, 64, 0x350, 4, 4, 4, 0x350, 0x350, 0x350, 32 };
    for (int k = 1; k < 12; k++) {
        uint32_t b = g_r[14 + 2 * k];
        if (p >= b && p < b + sz[k]) return d2TagBlob(g_r, k, (const void*)(uintptr_t)p);
    }
    if (p >= (uint32_t)emscripten_stack_get_end() && p < (uint32_t)emscripten_stack_get_base()) return 0xB3000000u;
    return p;
}
static uint32_t alloc(uint32_t size, uint32_t file, uint32_t line) { g_lg->rec(0x97de, 0, size, file, 0, 0, line, 0); return g_r[14 + 2 * 4]; }
static void copy(uint32_t d, uint32_t sv, uint32_t n) { g_lg->rec(0x9790, 0, 0, 0, 0, 0, tagp(d), tagp(sv), n); }
static void w2u(uint32_t d, uint32_t sv, uint32_t n) { g_lg->rec(0x1c038, 0, tagp(d), tagp(sv), 0, 0, n); }
static void u2w(uint32_t d, uint32_t sv, uint32_t n) { g_lg->rec(0x1c03c, 0, tagp(d), tagp(sv), 0, 0, n); }
static uint32_t rd16(uint32_t p) { g_lg->rec(0x9856, 0, 0, 0, 0, 0, tagp(p)); uint32_t i = g_n16++; return g_cfg[1 + (i < 2 ? i : 2)]; }
static uint32_t f9f40(uint32_t nm) { g_lg->rec(0x9f40, 0, 0, 0, tagp(nm)); return g_cfg[0]; }
static void f3a0(uint32_t buf, uint32_t node) { g_lg->rec(0xf3a0, tagp(buf), tagp(node)); }
static void cpy3(uint32_t d, uint32_t sv, uint32_t n) { g_lg->rec(0x13b0, 0, 0, 0, 0, 0, tagp(d), tagp(sv), n); }
static uint32_t f980e() { g_lg->rec(0x980e); return g_cfg[4 + (g_n80e++ ? 1 : 0)]; }
static uint32_t mk(uint32_t c, uint32_t t, uint32_t a, uint32_t b) { g_lg->rec(0x99ca, 0, 0, 0, 0, 0, c, t, tagp(a), tagp(b)); return g_cfg[6 + (g_n99ca++ ? 1 : 0)]; }
static void fail(uint32_t) {}
static void f9a0c(uint32_t h, uint32_t z) { g_lg->rec(0x9a0c, 0, 0, 0, 0, 0, h, z); }
static void f9a06(uint32_t h, uint32_t z) { g_lg->rec(0x9a06, 0, 0, 0, 0, 0, h, z); }
static void f9a2a(uint32_t h, uint32_t five) { g_lg->rec(0x9a2a, 0, h, five); }
static void fb600() { g_lg->rec(0xb600); }

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_c720(uint32_t* r) {
    D2HookLog lg(r[14]);
    g_lg = &lg; g_r = r; g_n16 = g_n80e = g_n99ca = 0;
    g_cfg = (const uint32_t*)(uintptr_t)r[14 + 2 * 11];
    auto blob = [&](int k) { return (uint32_t*)(uintptr_t)r[14 + 2 * k]; };
    D2lC720G G = { blob(5), (int32_t*)blob(6), blob(7) };
    static const D2lC720S S = { alloc, copy, w2u, u2w, rd16, f9f40, f3a0, cpy3, f980e, mk, fail, f9a0c, f9a06, f9a2a, fb600 };
    r[4] = (uint32_t)d2l_c720(r[0], r[1], r[2], r[3], r[5], r[6], &G, 0xB2000000u, &S);
}
