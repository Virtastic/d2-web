#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x69870: EAX = key (-1 = 0x1AF0), ECX = unit, EBX = out node*, stack (type, out value*).
// Walks data(+0x54)+0x28 (next +0x10) for node+0 == key: none = 1; node+4 == type = (*EBX = node, 0); else *EBX = 0,
// *out = 0x69090(unit, key), 0x69010(unit, node+8): non-zero = 1, zero = fatal 0x406. No unit/data is fatal 0x3F0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69870(uint32_t key, uint32_t unit, uint32_t outNode, uint32_t type, uint32_t outVal, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x3F0); return 0; }
    if (key == 0xFFFFFFFFu) key = e->call(0x1AF0, 0, 0, 0, 0, 0, 0, 0);
    uint32_t n = R(d, 0x28);
    for (; n; n = R(n, 0x10)) if (R(n, 0) == key) break;
    if (!n) return 1;
    if (R(n, 4) == type) { W(outNode, 0) = n; return 0; }
    W(outNode, 0) = 0;
    W(outVal, 0) = e->call(0x69090, unit, key, 0, 0, 0, 0, 0);
    if (e->call(0x69010, unit, 0, R(n, 8), 0, 0, 0, 0)) return 1;
    e->fatal(0x406); return 0;
}
