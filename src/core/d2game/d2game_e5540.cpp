// 1.13c D2Game+0xE5540 -- EAX (returned when nothing runs), ECX = quest node, EDX = event (+0xC unit, +0x14 and +0x18 words); no stack argument. Acts for the event code (16-bit word +0x14) 0xFD only, with the 16-bit word +0x18 at 0x1D1 or 0x1D2: 0xA57A(buffer, 0x10, 0) on the unit's quest-flag buffer for the node's game difficulty (0xA4C0 player data, byte +0x6D of the game at node +4, buffers from +0x10), then the holder at the game's +0x10F4 (its +8 word must be set, else the fatal path 0xA2DA/0xA2C2/exit) gives the buffer at +0xC and 0xA57A(that, 0x10, 0xD) is returned. A code 0xFD with another word returns the buffer.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe5540(GE& e, uint32_t eax, uint32_t node, uint32_t ev)
{
    if ((e.rd16s(ev + 0x14) & 0xffff) != 0xfd) return eax;
    const uint32_t w = e.rd16s(ev + 0x18) & 0xffff;
    const uint32_t game = e.rd32(node + 4);
    const uint32_t pd = e.h(0xA4C0, 0, 0, 0, 0, 0, e.rd32(ev + 0xc));
    const uint32_t buf = e.rd32(pd + 0x10 + 4 * e.rd8(game + 0x6d));
    if (w != 0x1d1 && w != 0x1d2) return buf;
    e.h(0xA57A, 0, 0, 0, 0, 0, buf, 0x10, 0);
    const uint32_t hd = e.rd32(e.rd32(node + 4) + 0x10f4);
    if (!e.rd32(hd + 8)) {   // retail logs through 0xA2DA/0xA2C2 and exits the process; not reproducible
        const uint32_t s = e.h(0xA2DA, 0, 0, 0, 0, 0, 0x820);
        e.h(0xA2C2, 0, 0, 0, 0, 0, 0, s);
        return s;
    }
    return e.h(0xA57A, 0, 0, 0, 0, 0, e.rd32(hd + 0xc), 0x10, 0xd);
}
}
