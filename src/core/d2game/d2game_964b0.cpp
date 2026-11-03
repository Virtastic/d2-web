// 1.13c D2Game+0x964B0: ebx = pool owner, stack p (ret 4). Walks the list at [p] (node +4 = id, next +0x14): 0x977E0(ecx = ebx, edx = id) then the pool free
// 0xA2EC(ecx = [ebx +0x1C], edx = node; file, 0x4A5, 0) per node, then both head words are cleared.
#include "d2game_seam4e.h"
extern "C" void d2game_964b0(uint32_t ebx, uint32_t p, const D2Seam* e)
{
    for (uint32_t n = d2rd32(p); n;) {
        const uint32_t id = d2rd32(n + 4), next = d2rd32(n + 0x14);
        e->call(0x977E0, 0, ebx, id);
        e->call(0xA2EC, 0, d2rd32(ebx + 0x1C), n, 0, 0, 0xB20FBA38u, 0x4A5, 0);
        n = next;
    }
    d2wr32(p, 0);
    d2wr32(p + 4, 0);
}
