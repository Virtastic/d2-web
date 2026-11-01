// 1.13c D2Game+0x8F6C0 -- find or add a vendor-chain node for a unit.
// EAX = unit (null -> id -1), EDI = record (chain head at +0x18, node: id +0, next +8), one stack argument (ret 4) = pool owner
// (pool at +0x1C). A miss allocates a 12-byte node (0xA2E6: ECX = pool, EDX = 12, stack file/line 0x4ED/0) pushed on the head.
#include "d2game_8d6_common.h"
struct D2Game8F6C0Env {
    uint32_t (*allocBlock)(uint32_t pool, uint32_t size, uint32_t line);   // 0xA2E6
};
// 1.13c D2Game+0x8F6C0
uint32_t d2game_port_8f6c0(uint32_t unit, uint32_t rec, uint32_t owner, const D2Game8F6C0Env* env)
{
    const uint32_t id = unit ? d2g8::r32(unit, 0xC) : 0xFFFFFFFFu;
    for (uint32_t n = d2g8::r32(rec, 0x18); n; n = d2g8::r32(n, 8)) if (d2g8::r32(n, 0) == id) return n;
    const uint32_t node = env->allocBlock(d2g8::r32(owner, 0x1C), 0xC, 0x4ED);
    d2g8::w32(node, 0, id);
    d2g8::w32(node, 8, d2g8::r32(rec, 0x18));
    d2g8::w32(rec, 0x18, node);
    return node;
}
