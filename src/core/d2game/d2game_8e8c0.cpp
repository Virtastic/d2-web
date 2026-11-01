// 1.13c D2Game+0x8E8C0 -- run 0x8E6F0 on every node of an owner's list that matches a type and id.
// EAX = owner (may be null), ECX = game (EDI for the callee), EBX = type, stack (ret 4) = id. List head owner+0xDC, link +0x24,
// node byte 0 = type (compared with the full EBX), node +0x2C = id. The link is read before the callee runs.
#include "d2game_8d6_common.h"
struct D2Game8E8C0Env {
    void (*sub8E6F0)(uint32_t node, uint32_t game);   // 0x8E6F0 (EAX = node, EDI = game)
};
// 1.13c D2Game+0x8E8C0
void d2game_port_8e8c0(uint32_t owner, uint32_t game, uint32_t type, uint32_t id, const D2Game8E8C0Env* env)
{
    if (!owner) return;
    uint32_t node = d2g8::r32(owner, 0xDC);
    while (node) {
        const uint32_t next = d2g8::r32(node, 0x24);
        if ((uint32_t)d2g8::r8(node, 0) == type && id == d2g8::r32(node, 0x2C)) env->sub8E6F0(node, game);
        node = next;
    }
}
