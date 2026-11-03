// 1.13c D2Client+0x69180: thiscall, ECX = node, stack = unit. Finds the node in
// data(+0x54)+0x2C (next +0xA4); frees its handle (+0x40, callee 0xA8630 ESI), unlinks it, calls 0x689F0(EAX = unit, EBX = node,
// stack = data) for a type-8 node, then frees it (line 0x1E1). No unit/data is fatal (line 0x1BF).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69180(uint32_t node, uint32_t unit, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x1BF); return; }
    uint32_t prev = 0, n = R(d, 0x2C);
    for (; n; prev = n, n = R(n, 0xA4))
        if (n == node) break;
    if (!n) return;
    if (R(n, 0x40)) { e->call(0xA8630, 0, 0, 0, 0, R(n, 0x40), 0, 0); W(n, 0x40) = 0; }
    if (prev) W(prev, 0xA4) = R(n, 0xA4); else W(d, 0x2C) = R(n, 0xA4);
    if (R(n, 0) == 8) e->call(0x689F0, unit, 0, 0, n, 0, d, 0);
    e->freeMem(n, 0x1E1);
}
