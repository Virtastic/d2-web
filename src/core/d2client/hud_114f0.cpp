#include <cstdint>
#include <emscripten.h>

// 1.13c D2Client+0x114F0 (stdcall param): which refreshes run. Returns the mask: bit0 call(0x6FBA8C50), bit1 call(0x6FBA8C44),
// bit2 call(0x6FBA8C38), bit3 call 0x6FB6E920, bit4 [0x6FBCC540] cleared. sm = GetSystemMetrics(0x4B), c [0x6FBCC3CC],
// m [0x6FBC9898], s [0x6FBC988C], u [0x6FBC9890]. The six 0x3CA00 calls and the ordinal-10048 checks always run and are not modeled.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_114f0(uint32_t param, uint32_t sm, uint32_t c, uint32_t m, uint32_t s, uint32_t u) {
    const bool pnl = m == 2 && (s == 4 || s == 5), any = m == 1 || pnl;
    uint32_t r = 0;
    if (sm && !c) return param ? 24u : 0u;
    if (sm && ((m == 1 && u) || pnl)) r |= 1;
    if (c && m == 2 && s == 5) r |= 2;
    if (c && any) r |= 4;
    return r | (param ? 24u : 0u);
}
