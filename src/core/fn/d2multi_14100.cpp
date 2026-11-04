// 1.13c D2Multi+0x14100 probe (stdcall, one ignored stack word; r[4] = 0/1). Blobs (r[14+2k]): k0 gh (DAT_6fa09ccc.., 0x10 bytes), k1 cnt (DAT_6fa09dcc), k2 tb (the record
// table at DAT_6f9fa598), k3 hl (hook log), k4 ua / k5 ub (the two wide strings 7C50 hands out), k6 rv (the 0x3C-byte record 139B0 writes through EBX), k7 cfg
// (dwords: 7C62 answer, 7C50 first call gives ua, 7E2A answer, 139B0 answer). unicode2Win runs for real in the capture; the probe narrows ASCII, which is what it does for it.
#include "d2multi_probe.h"
uint32_t d2multi_14100(const D2mEnv& e, void (*u2w)(char*, const uint16_t*, int), const uint32_t* h, const int32_t* cnt, const uint8_t* tbl);
static void u2w_ascii(char* d, const uint16_t* s, int n) {
    int i = 0;
    for (; i < n - 1 && s[i]; i++) d[i] = (char)s[i];
    d[i] = 0;
}
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2multi_14100(uint32_t* r) {
    const uint32_t gh = r[14], cnt = r[16], tb = r[18], hl = r[20], ua = r[22], ub = r[24], rv = r[26], cfg = r[28];
    uint32_t c[4]; memcpy(c, (const void*)(uintptr_t)cfg, 16);
    uint32_t w[15]; memcpy(w, (const void*)(uintptr_t)rv, 60);
    const uint32_t k = (0x13u << 24) | 0x139b0;
    uint32_t sc[48]; int n = 0;
    auto put = [&](uint32_t key, uint32_t v) { sc[n++] = key; sc[n++] = v; };
    put(0x7c62, c[0]);
    put(0x7c50, c[1] ? ua : 0); put(0x7c50, ub);
    put(0x7e2a, c[2]); put(0x7bd8, 0x2001); put(0x139b0, c[3]); put(0x7ea8, 0x1479);
    for (int i = 0; i < 15; i++) put(k, w[i]);
    put(0, 0);
    D2mProbe p(hl, sc);
    r[4] = d2multi_14100(p.env, &u2w_ascii, (const uint32_t*)(uintptr_t)gh, (const int32_t*)(uintptr_t)cnt, (const uint8_t*)(uintptr_t)tb);
}
