// 1.13c D2Game+0x8E540 -- take a zeroed 0x30-byte node from the game's slab chain.
// ESI = pool owner (slab chain head at +0xA1C), one stack argument (ret 4) handed to 0x8E420 in EAX when every slab is empty.
// Slab: free-list head at +0x7080 (node link at +0x1C), next slab at +0x7084. A new slab is linked in after the head.
#include "d2game_8d6_common.h"
struct D2Game8E540Env {
    uint32_t (*allocSlab)(uint32_t arg);   // 0x8E420 (EAX = arg)
};
// 1.13c D2Game+0x8E540
uint32_t d2game_port_8e540(uint32_t owner, uint32_t arg, const D2Game8E540Env* env)
{
    uint32_t slab = d2g8::r32(owner, 0xA1C);
    while (slab) {
        if (d2g8::r32(slab, 0x7080)) goto found;
        slab = d2g8::r32(slab, 0x7084);
    }
    slab = env->allocSlab(arg);
    {
        const uint32_t head = d2g8::r32(owner, 0xA1C);
        d2g8::w32(slab, 0x7084, d2g8::r32(head, 0x7084));
        d2g8::w32(d2g8::r32(owner, 0xA1C), 0x7084, slab);
    }
found:
    const uint32_t node = d2g8::r32(slab, 0x7080);
    d2g8::w32(slab, 0x7080, d2g8::r32(node, 0x1C));
    std::memset(d2g8::PW(node), 0, 0x30);
    return node;
}
