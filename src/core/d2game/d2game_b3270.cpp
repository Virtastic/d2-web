// 1.13c D2Game+0xB3270 -- set the unit's wait counter from its class record (ESI = unit, stack = event record, ret 4).
// Written from the 1.13c disassembly as a rule over the seam environment. Not a monster (type != 1): 0xACE8(unit, 8), unit+0x48 = 0x500.
// A monster with a 0x1320(class) record: event byte +0x11 != 0xE sets unit+0x44 = record byte +0x109 << 8; else
// 0xACE8(unit, byte - 1) and unit+0x48 = (byte + 1) << 8. A null unit faults in retail and is not modelled.
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fb3270(GE& e, uint32_t unit, uint32_t ev)
{
    if (unit && e.rd32(unit) == 1) {
        const uint32_t rec = e.h(0x1320, e.rd32(unit + 4));
        if (!rec) return 0;
        const uint32_t b = e.rd8(rec + 0x109);
        if (e.rd8(ev + 0x11) != 0xE) { e.wr32(unit + 0x44, b << 8); return b << 8; }
        const uint32_t r = e.h(0xACE8, 0, 0, 0, 0, 0, unit, b - 1);
        e.wr32(unit + 0x48, (b + 1) << 8);
        return r;
    }
    const uint32_t r = e.h(0xACE8, 0, 0, 0, 0, 0, unit, 8);
    e.wr32(unit + 0x48, 0x500);
    return r;
}
}
