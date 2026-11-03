// 1.13c D2Client+0x69010: EAX = unit, EDX = key. Unlinks and frees (line 0x3D0)
// the first node of data(+0x54)+0x28 whose +8 equals the key; returns 1, or 0 when none. No unit/data is fatal (line 0x3C8).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2cf_69010(uint32_t unit, uint32_t key, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x3C8); return 0; }
    uint32_t prev = 0, n = R(d, 0x28);
    for (; n; prev = n, n = R(n, 0x10))
        if (R(n, 8) == key) break;
    if (!n) return 0;
    uint32_t next = R(n, 0x10);
    e->freeMem(n, 0x3D0);
    if (prev) W(prev, 0x10) = next; else W(d, 0x28) = next;
    return 1;
}
