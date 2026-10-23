// 1.13c D2Client+0xA60D0: unit (EAX) -> A4E20 lookup in the per-mode table; unit flag bit 10 gates it.
// u = the 32-bit unit words. Null unit: retail reports via Ordinal_10265/10024 then _exit(-1); ours returns 0xFFFFFFFF (caller must not continue).
#include "client_host.h"
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_a60d0(D2cHost h, const uint32_t* u) {
    if (!u) return 0xFFFFFFFFu;
    if (!((u[200 / 4] >> 10) & 1)) return 0;
    const uint32_t mode = u[0x94 / 4];
    const uint32_t tab = mode == 3 ? 0x10A008u : 0x10A608u + mode * 0x200u;   // 0x6FBBA008 / 0x6FBBA608 + mode*0x80 dwords, as RVAs
    D2cCall k = { 0xA4E20, mode, u[0x98 / 4], 0xB2000000u | tab, 0, 0, { 0, 0, 0, 0 }, { 0, 0, 0, 0 } };   // register assignment unverified
    return h(&k);
}
