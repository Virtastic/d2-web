// 1.13c D2Client+0x69700: EAX = unit. Frees the data(+0x54)+0x28 list (line 0x81A),
// runs 0x68FD0(EAX = data) over the +0x2C list, frees the data block (line 0x820). No unit/data is fatal (line 0x814).
#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_69700(uint32_t unit, const Env* e) {
    uint32_t d = unit ? R(unit, 0x54) : 0;
    if (!d) { e->fatal(0x814); return; }
    for (uint32_t n = R(d, 0x28); n;) { uint32_t next = R(n, 0x10); e->freeMem(n, 0x81A); n = next; }
    e->call(0x68FD0, d, 0, 0, 0, 0, 0, 0);
    e->freeMem(d, 0x820);
}
