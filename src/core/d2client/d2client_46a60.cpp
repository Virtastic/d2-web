// D2Client 0x46A60: drop the unit keyed ECX, then clear the pending 0x1197AC/0x1197B0 pair
// (callers 0x4B8C8, 0x47B20, 0x7D3C0, 0x7EA90, 0x4A4F0).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
// 1.13c D2Client+0x46A60 ; key = ECX, hash = the 128 heads at 0x10A808 (records: +0 type, +0xC key, +0xE4 next), g97ac/g97b0 =
// the dwords at 0x1197AC/0x1197B0, list = [0x11BF30] (nodes {item, next}, item+8 compared to g97b0). Pointers are the game's
// 32-bit words, read through `at`. A matching record whose type is not 1 exits the process in retail (fatal). 0x460A0 (EAX =
// record) and 0x88550 (ECX = item) go through host.
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_46a60(uint32_t key, const uint32_t* hash, uint32_t* g97ac, uint32_t* g97b0, uint32_t list,
                                                    uint32_t* (*at)(uint32_t), void (*fatal)(), D2cHost host) {
    for (uint32_t a = hash[key & 0x7F]; a;) {
        const uint32_t* u = at(a);
        if (u[3] == key) {
            if (u[0] != 1) { fatal(); return; }
            D2cCall c = {}; c.rva = 0x460A0; c.eax = a; host(&c);
            break;
        }
        a = u[0x39];
    }
    if (*g97ac && *g97b0)
        for (uint32_t n = list; n; n = at(n)[1]) {
            uint32_t item = at(n)[0];
            if (at(item)[2] == *g97b0) { D2cCall c = {}; c.rva = 0x88550; c.ecx = item; host(&c); break; }
        }
    *g97ac = 0; *g97b0 = 0;
}
