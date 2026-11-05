// d2game+0xF0480 probe: r[0] = ECX game, r[1] = EDX p1, r[2] = stack p2. Blobs lg 0, sc 1, wl 2 (packet log).
// The 0x8A3E0 send is recorded like the capture's `hook 0x8a3e0 2 aB` + `hookwlog`; the EDI arguments of 0x9BEB0, 0xF0220 and 0xF0130 are REGS `J` records.
#include <emscripten.h>
#include <cstdint>
#include "d2game_seam_probe.h"
#include "d2game_hk2.h"
extern "C" void d2game_f0480(uint32_t, uint32_t, uint32_t, const D2Seam*);
static HookEnv* g_e; static HkWlog* g_w;
static uint32_t thunk(void* c, uint32_t rva, uint32_t a, uint32_t cc, uint32_t d, uint32_t b, uint32_t s, uint32_t s0, uint32_t s1, uint32_t s2,
                      uint32_t s3, uint32_t s4, uint32_t s5, uint32_t s6, uint32_t s7) {
    if (rva == 0x8A3E0) { hk_send(*g_e, *g_w, (const void*)(uintptr_t)a, (const void*)(uintptr_t)s0, s1); return 0; }
    return SeamProbe::thunk(c, rva, a, cc, d, b, s, s0, s1, s2, s3, s4, s5, s6, s7);
}
static void edi(void*, uint32_t rva, uint32_t v) { g_e->edi(rva, v); }
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2game_f0480(uint32_t* r) {
    uint32_t* lg = (uint32_t*)(uintptr_t)r[14];
    SeamProbe p(lg, (const uint32_t*)(uintptr_t)r[16]); D2Seam s = p.seam(); s.fn = thunk; s.ej = edi;
    HkWlog w; w.init(r[18]); g_e = &p.e; g_w = &w;
    d2game_f0480(r[0], r[1], r[2], &s);
    r[4] = 0;
    hk_tag(r, lg);
}
