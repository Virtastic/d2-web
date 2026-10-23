// D2Client+0xA3D20: nested-window/dialog teardown; env-seam style.
#include <cstdint>
#include <emscripten.h>
struct D2cA3D20Env {
    uint32_t* depth;        // 0x6FBCBC08 nesting counter
    uint32_t* flagAda8;     // 0x6FBAADA8
    uint32_t* cur;          // 0x6FBCBC00 current window ptr (guest addr)
    uint32_t* cbc04;        // 0x6FBCBC04
    uint32_t* cc2fc;        // 0x6FBCC2FC
    uint32_t* g90ea0;       // 0x6FB90EA0
    uint32_t (*rd32)(uint32_t addr);   // guest read
    uint32_t (*ord10048)();            // Ordinal_10048 (unverified name)
    void (*f3CCC0)(uint32_t a, uint32_t b);   // D2Client+0x3CCC0 (eax=ord result)
    void (*f3CC60)(uint32_t a, uint32_t b);   // D2Client+0x3CC60
    uint32_t (*f42090)();              // D2Client+0x42090
    void (*ord10043)(uint32_t id, uint32_t z);// Ordinal_10043 (unverified)
    void (*ord10065)(uint32_t w);             // Ordinal_10065 (unverified)
    void (*fatal)(uint32_t msgId);     // Ordinal_10265(0x441) -> Ordinal_10024 -> _exit(-1)
};
extern "C" {
// 1.13c D2Client+0xA3D20
EMSCRIPTEN_KEEPALIVE void d2c_a3d20(const D2cA3D20Env* e) {
    *e->depth -= 1;
    if (*e->depth != 0) { e->fatal(0x441); return; }
    if (*e->flagAda8) e->f3CCC0(e->ord10048(), 0x6fb80780);
    e->f3CC60(e->ord10048(), 0x6fb8e858);
    if (e->f42090()) e->f3CC60(e->ord10048(), 0x6fb8e900);
    uint32_t w = *e->cur;
    uint32_t n = e->rd32(w + 0xb08);
    *e->g90ea0 = 1; *e->cc2fc = 0; *e->cbc04 = 0;
    while (n) { n = e->rd32(n + 4); e->ord10043(0x430, 0); }
    if (e->rd32(w + 0xc) != 0) {
        uint32_t h = e->rd32(w + 8);
        if (h) { e->ord10065(h); e->ord10043(0x7a, 0); }  // decompile: second call gated on h != 0 after ord10065
    }
    uint32_t next = e->rd32(w + 0xb0c);
    e->ord10043(0x43b, 0);
    *e->cur = next;
}
}
