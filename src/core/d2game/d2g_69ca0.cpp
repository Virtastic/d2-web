// 1.13c D2Game+0x69CA0 -- EDX = unit; no result. Written from the 1.13c disassembly.
// n = byte unit->data(+0x14)+4, rec = 0xB156(unit), k = rec+0x180. n / k >= 2 asserts (line 0xDC); the counter is bumped, and
// a counter already past the byte 2k asserts (line 0xE3; neither is driven). A bumped count <= 2k that is a multiple of k
// (byte k = 2k >> 1) sets mode 2 - count / k (0xA4A2). Then 0xA4DE(unit) and unit+0xC4 |= 1.
#include "../fn/hookenv.h"
#include "d2game_fatal.h"
namespace d2g {
void f69ca0(HookEnv& e, uint32_t unit)
{
    const uint32_t bl = e.rd8(e.rd32(unit + 0x14) + 4);
    const uint32_t rec = e.h(0xB156, 0, 0, 0, 0, 0, unit);
    const int32_t k = (int32_t)e.rd32(rec + 0x180);
    if ((int32_t)bl / k >= 2) { d2gfatal::Fatal(e, 0xDC, 0xB2000000u); return; }
    const uint32_t al = (bl + 1) & 0xFF;
    e.wr8(e.rd32(unit + 0x14) + 4, al);
    const uint32_t dl = (uint32_t)(k << 1) & 0xFF;
    if ((int32_t)(dl - bl) < 0) {
        e.wr8(e.rd32(unit + 0x14) + 4, dl);
        d2gfatal::Fatal(e, 0xE3, 0xB2000000u);
        return;
    }
    if (al <= dl && (int32_t)al % (int32_t)(dl >> 1) == 0)
        e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 2 - (uint32_t)((int32_t)al / k));
    e.h(0xA4DE, 0, 0, 0, 0, 0, unit);
    e.wr32(unit + 0xC4, e.rd32(unit + 0xC4) | 1);
}
}
