// 1.13c D2Multi+0x13540 probe: r[0]=EAX name, r[1]=EDI source string, r[2]=a, r[3]=b (stack, ret 8). Blobs (r[14+2k]; k>=8 at r[30+2(k-8)]): k0 nm, k1 src, k2 vt (DAT_6fa0917c,
// +0x78 = the recorder callback at the unused RVA 0x1000), k3 hl (hook log), k4 gh (DAT_6fa09150..5c), k5 gw (DAT_6fa09d64.., 0x8c bytes), k6 g94 (DAT_6fa0a094),
// k7 g118 (DAT_6fa09118), k8 g0d0 (DAT_6fa0a0d0), k9 p (what 7BA8 returns when cfg[0]), k10/11 tokA/tokB (what 1274 returns, cfg[1] of them), k12 cfg.
#include "d2multi_probe.h"
void d2multi_13540(const D2mEnv& e, uint32_t name, const char* src, uint32_t a, uint32_t b, const uint32_t* h, uint32_t* w,
                   const uint32_t* g94, uint32_t* g118, const uint32_t* g0d0);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2multi_13540(uint32_t* r) {
    const uint32_t vt = r[18], hl = r[20], gh = r[22], gw = r[24], g94 = r[26], g118 = r[28], g0d0 = r[30], pb = r[32], ta = r[34], tb = r[36], cfg = r[38];
    const uint32_t nfound = rd32m(cfg), ntok = rd32m(cfg + 4);
    const uint32_t sc[] = { 0x7ba8, nfound ? pb : 0, 0x1274, ntok > 0 ? ta : 0, 0x1274, ntok > 1 ? tb : 0, 0x1274, 0,
                            0x7caa, 0x1001, 0x7caa, 0x1002, 0x7caa, 0x1003, 0x7caa, 0x1004, 0x7caa, 0x1005, 0x7caa, 0x1006, 0x7caa, 0x1007, 0x7caa, 0x1008,
                            0x7ea8, 0x1479, 0, 0 };
    D2mProbe p(hl, sc);
    wr32m(vt + 0x78, 0xB2001000u);
    d2multi_13540(p.env, r[0], (const char*)(uintptr_t)r[1], r[2], r[3], (const uint32_t*)(uintptr_t)gh, (uint32_t*)(uintptr_t)gw,
                  (const uint32_t*)(uintptr_t)g94, (uint32_t*)(uintptr_t)g118, (const uint32_t*)(uintptr_t)g0d0);
    r[4] = 0;
}
