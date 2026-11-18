// D2Client 0x46A20: the buy/sell panel reset (caller 0xC0250).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x46A20 ; g23e8 = the u16 at 0xF23E8, g9662 = the byte at 0x119662, recs = the 0x16-byte records at 0xF37A8,
// n = their count [0xF3B9C]. Callees 0x46640 and 0x45EF0 (no arguments) go through host, in retail's order.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46a20(uint16_t* g23e8, uint8_t* g9662, uint8_t* recs, int32_t n, D2cHost host) {
    *g23e8 = 0xFFFF;
    D2cCall c = {}; c.rva = 0x46640; host(&c);
    c = {}; c.rva = 0x45EF0; host(&c);
    *g9662 = 0;
    for (int32_t i = 0; i < n; ++i) { recs[i * 0x16 + 0x11] = 0; recs[i * 0x16 + 0x12] = 0; }
}
