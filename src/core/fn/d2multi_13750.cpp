// 1.13c D2Multi+0x13750 probe: r[0]=str, r[1]=a, r[2]=b (stdcall, ret 0xC). Blobs (r[14+2k]): k0 str, k1 gs (DAT_6fa09f14), k2 vt (DAT_6fa0917c, +0x78 = the recorder
// callback at the unused RVA 0x1000), k3 hl (hook log), k4 gh (DAT_6fa09150..5c), k5 gw (DAT_6fa09d78.., 0x78 bytes), k6 g94 (DAT_6fa0a094), k7 g118 (DAT_6fa09118),
// k8 cap (the three caption dwords the 7C44 recorder writes into each of the three local buffers).
#include "d2multi_probe.h"
void d2multi_13750(const D2mEnv& e, uint32_t esi, const uint32_t* h, uint32_t* w, const uint32_t* g94, uint32_t* g118,
                   const char* str, uint32_t a, uint32_t b);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2multi_13750(uint32_t* r) {
    const uint32_t gs = r[16], vt = r[18], hl = r[20], gh = r[22], gw = r[24], g94 = r[26], g118 = r[28], cap = r[30];
    uint32_t c[3]; memcpy(c, (const void*)(uintptr_t)cap, 12);
    const uint32_t k = (0x12u << 24) | 0x7c44;
    const uint32_t sc[] = { 0x7caa, 0x1001, 0x7caa, 0x1002, 0x7caa, 0x1003, 0x7caa, 0x1004, 0x7caa, 0x1005, 0x7caa, 0x1006, 0x7caa, 0x1007, 0x7caa, 0x1008,
                            0x7ea8, 0x1479, k, c[0], k, c[1], k, c[2], 0, 0 };
    D2mProbe p(hl, sc);
    wr32m(vt + 0x78, 0xB2001000u);
    d2multi_13750(p.env, gs + 0x89, (const uint32_t*)(uintptr_t)gh, (uint32_t*)(uintptr_t)gw, (const uint32_t*)(uintptr_t)g94,
                  (uint32_t*)(uintptr_t)g118, (const char*)(uintptr_t)r[0], r[1], r[2]);
    r[4] = 0;
}
