#include <emscripten.h>
#include "d2cf.h"
using namespace d2cf;
// 1.13c D2Client+0x6A180: ESI = unit, EAX = index. Gfx prologue (line 0x498) gives the kind; index 0xE takes the kind.
// 0x1C10(unit id, idx) zero returns; index 0xC first runs 0x18EA0(unit), 0x18CB0(unit, 0); then 0x6A000(unit, idx).
extern "C" EMSCRIPTEN_KEEPALIVE void d2cf_6a180(uint32_t unit, uint32_t idx, const Env* e) {
    uint32_t kind = gfxPro(e, unit, 0x498);
    if (idx == 0xE) idx = kind;
    if (!e->call(0x1C10, unitId(unit), 0, idx, 0, 0, 0, 0)) return;
    if (idx == 0xC) { e->call(0x18EA0, unit, 0, 0, 0, 0, 0, 0); e->call(0x18CB0, unit, 0, 0, 0, 0, 0, 0); }
    e->call(0x6A000, unit, 0, 0, 0, 0, 0, 0);
}
