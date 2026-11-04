// D2Launch 1.13c +0x122C0: replay probe over d2launch_sub_122C0 (src/core/d2launch/d2launch_n57.cpp).
// No arguments. Blobs: lg r[14] hook log, gv r[16] (+0 DAT 254FC, +4 DAT 2529C, +8 DAT 25504 pins), w1 r[18], w2 r[20] (UTF-16 strings; a 4-byte ff blob means the lookup returns null).
#include <emscripten.h>
#include <cstdint>
#include <cstddef>
void d2launch_sub_122C0(const void* (*gettext)(uint32_t), size_t (*ulen)(const void*), void (*update)(uint32_t, uint32_t),
                        uint32_t g254fc, uint32_t g2529c, uint32_t g25504);
static uint32_t* s_lg; static const void* s_w[2]; static int s_n;
static void rec(uint32_t rva, uint32_t ecx, uint32_t edx) {
    uint32_t* w = s_lg + 1 + s_lg[0]++ * 10;
    const uint32_t v[10] = { rva, 0, ecx, edx, 0, 0, 0, 0, 0, 0 };
    for (int i = 0; i < 10; ++i) w[i] = v[i];
}
static const void* text(uint32_t ecx) { rec(0x98c2, ecx, 0); const void* q = s_w[s_n++ & 1]; return *(const uint16_t*)q == 0xffff ? nullptr : q; }
static size_t ulen(const void* p) { const uint16_t* h = (const uint16_t*)p; size_t n = 0; while (h[n]) ++n; return n; }
static void upd(uint32_t ecx, uint32_t ok) { rec(0x9862, ecx, ok); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2launch_122c0(uint32_t* r) {
    s_lg = (uint32_t*)(uintptr_t)r[14];
    const uint32_t* gv = (const uint32_t*)(uintptr_t)r[16];
    s_w[0] = (const void*)(uintptr_t)r[18]; s_w[1] = (const void*)(uintptr_t)r[20]; s_n = 0;
    d2launch_sub_122C0(text, ulen, upd, gv[0], gv[1], gv[2]);
    r[4] = 0;
}
