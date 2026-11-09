// 1.13c D2Client+0xABC20 runUnitQueue, written from the disassembly: stdcall(unit), ret 4. Runs the unit's queued packet records
// (header at unit+0xD8: cap, count, ?, buf; record stride 0x11BBD8) through the handler table 0xDDE60 (175 x {id, size, handler}):
// per record the 0xD246 length call (ECX = record, the short written into the argument slot) gates it, the type byte must be below
// 0xAF, a size other than -1 must equal that short, the handler (fastcall ECX = unit, EDX = record) must be set; then clears the records.
#include "cl_genv.h"
using namespace cl;
extern "C" void d2c13_abc20(uint32_t unit, const Env& e, const GEnv& g) {
    if (!unit) { Fatal(e, g, 0x8a5); return; }
    const uint32_t hdr = e.R(unit + 0xd8);
    if (!hdr) return;
    if ((int32_t)e.R(hdr + 4) > (int32_t)e.R(hdr)) { Fatal(e, g, 0x1524); return; }
    uint32_t slot = unit;   // retail reuses the argument slot as the out-short of 0xD246
    for (uint32_t i = 0; (int32_t)i < (int32_t)e.R(hdr + 4); i++) {
        const uint32_t ent = e.R(hdr + 0xc) + i * e.R(g.G(0x11bbd8));
        if (!e.H(0xd246, Stk(&slot), 0, 0, 0, 0, 0, ent)) return;
        const uint32_t b = B(ent);
        if (b >= 0xaf) { Fatal(e, g, 0x1530); return; }
        const uint32_t sz = e.R(g.G(0xdde64 + 12 * b));
        if (sz != 0xffffffffu && (slot & 0xffff) != sz) { Fatal(e, g, 0x1533); return; }
        const uint32_t fn = e.R(g.G(0xdde68 + 12 * b));
        if (!fn) { Fatal(e, g, 0x1535); return; }
        e.H(fn & 0xffffff, 0, 0, 0, 0, 0, 0, unit, ent);
    }
    if (e.R(hdr + 4)) Zero(e.R(hdr + 0xc), e.R(hdr + 4) * e.R(g.G(0x11bbd8)));
    e.W(hdr + 4, 0);
}
