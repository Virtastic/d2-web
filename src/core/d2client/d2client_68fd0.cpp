// 1.13c D2Client+0x68FD0: EAX = unit data (+0x2C node list, next +0xA4). Per node:
// handle at +0x40, when set, goes to callee 0xA8630 (ESI), then the node is freed (line 0x690).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_68fd0(uint32_t unit, const Env* e) {
    for (uint32_t n = R(unit, 0x2C); n;) {
        uint32_t h = R(n, 0x40), next = R(n, 0xA4);
        if (h) e->call(0xA8630, 0, 0, 0, 0, h, 0, 0);
        e->freeMem(n, 0x690);
        n = next;
    }
}
