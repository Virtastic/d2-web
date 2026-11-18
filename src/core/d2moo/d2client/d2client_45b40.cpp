// D2Client 0x45B40: a yes/no test on the unit in ESI, made from D2Common unit/stat queries
// (callers 0x46230, 0x47D60).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
static uint32_t q45b40(D2cHost host, uint32_t rva, uint32_t u, uint32_t a = 0, uint32_t b = 0) {
    D2cCall c = {}; c.rva = rva; c.s[0] = u; c.s[1] = a; c.s[2] = b; return host(&c);
}
// 1.13c D2Client+0x45B40 ; unit = ESI, ecx = the caller's ECX, which `push ecx` turns into the out slot handed to the thunk 0xCE20
// (only that callee writes it; the host returns the slot in out[0]). Thunks, one stack word unless noted: 0xC220 (== 4 -> 0),
// 0xCE14 (0 -> 0), 0xC8BC/0xC916, 0xC1C6 (unit, stat, 0: stat 0x48 then 0x46), 0xC73C/0xC91C, 0xCE20 (unit, &slot). Signed compares.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_45b40(uint32_t unit, uint32_t ecx, D2cHost host) {
    if (q45b40(host, 0xC220, unit) == 4) return 0;
    if (!q45b40(host, 0xCE14, unit)) return 0;
    if (q45b40(host, 0xC8BC, unit)) {
        int32_t lim = (int32_t)q45b40(host, 0xC916, unit);
        if (lim && (int32_t)q45b40(host, 0xC1C6, unit, 0x48, 0) < lim) return 1;
    }
    if (q45b40(host, 0xC73C, unit)) {
        int32_t v = (int32_t)q45b40(host, 0xC1C6, unit, 0x46, 0);
        if (v < (int32_t)q45b40(host, 0xC91C, unit)) return 1;
    }
    D2cCall c = {}; c.rva = 0xCE20; c.s[0] = unit; c.s[1] = 0xB3000000u; c.out[0] = ecx;
    if (host(&c) && c.out[0] == 0) return 1;
    return 0;
}
