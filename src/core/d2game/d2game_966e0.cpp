// 1.13c D2Game+0x966E0: edx = x, stack p (ret 4). sender = (x != 0 && [x] == 0) ? [0xA4C0(x) +0x9C] : 0. With p +0x10 and +0x14 both zero it runs
// 0x8B290(eax = sender, ecx = [p] with cl = 1, edx = [p +0xC]; sender, [p +0xC], [p], [p +4], word [p +8]), otherwise 0x8B220(eax = sender, ecx = [p +0xC], edx = [p];
// sender, [p +0xC], [p +4], word [p +8], [p +0x10], [p +0x14]). Returns the callee's eax.
#include "d2game_seam4e.h"
extern "C" uint32_t d2game_966e0_seam(uint32_t x, uint32_t p, const D2Seam* e)
{
    uint32_t sender = 0;
    if (x && d2rd32(x) == 0) sender = d2rd32(e->call(0xA4C0, 0, 0, 0, 0, 0, x) + 0x9C);
    const uint32_t w8 = d2rd16(p + 8);
    if (d2rd32(p + 0x10) == 0 && d2rd32(p + 0x14) == 0)
        return e->call(0x8B290, sender, (d2rd32(p) & 0xFFFFFF00u) | 1, d2rd32(p + 0xC), 0, 0, sender, d2rd32(p + 0xC), d2rd32(p), d2rd32(p + 4), w8);
    return e->call(0x8B220, sender, d2rd32(p + 0xC), d2rd32(p), 0, 0, sender, d2rd32(p + 0xC), d2rd32(p + 4), w8, d2rd32(p + 0x10), d2rd32(p + 0x14));
}
