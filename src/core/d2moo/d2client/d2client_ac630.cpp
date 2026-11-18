// 1.13c D2Client+0xAC630 queuePush, written from the disassembly: EAX = length, ECX = unit, [esp+4] = source; ret 4.
// Appends one record (length bytes of source) to the unit's packet queue, growing it through 0xABDC0 when there is no header
// or the next push would fill it (count + 1 == cap); a record longer than the stride (0x11BBD8) is fatal. Returns the header.
#include "cl_genv.h"
using namespace cl;
extern "C" uint32_t d2c13_abdc0(uint32_t unit, const Env& e, const GEnv& g);
extern "C" uint32_t d2c13_ac630(uint32_t len, uint32_t unit, uint32_t src, const Env& e, const GEnv& g) {
    if (!unit) { Fatal(e, g, 0x8a5); return 0; }
    uint32_t hdr = e.R(unit + 0xd8);
    if (!hdr || e.R(hdr + 4) + 1 == e.R(hdr)) { hdr = d2c13_abdc0(unit, e, g); if (!hdr) return 0; }
    const uint32_t st = e.R(g.G(0x11bbd8));
    if ((int32_t)len > (int32_t)st) { Fatal(e, g, 0x127f); return 0; }
    Copy(e.R(hdr + 0xc) + e.R(hdr + 4) * st, src, len);
    e.W(hdr + 4, e.R(hdr + 4) + 1);
    return hdr;
}
