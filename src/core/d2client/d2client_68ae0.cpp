// 1.13c D2Client+0x68AE0: EAX = container (+0x2C node list), ECX = node. Copies the
// state of the first other type-8 node of the same +0x38 key; a type-8 node with another key and +0x3C set clears node+0x3C.
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_68ae0(uint32_t cont, uint32_t node) {
    for (uint32_t p = R(cont, 0x2C);; p = R(p, 0xA4)) {
        if (!p) { W(node, 0x3C) = 1; W(node, 8) = 0; return; }
        if (R(p, 0) == 8 && p != node) {
            if (R(p, 0x38) == R(node, 0x38)) { W(node, 0x3C) = R(p, 0x3C); W(node, 8) = R(p, 8); return; }
            if (R(p, 0x3C)) { W(node, 0x3C) = 0; return; }
        }
    }
}
