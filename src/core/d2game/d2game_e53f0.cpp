// 1.13c D2Game+0xE53F0 -- EAX (kept when the code differs), ECX = quest node, EDX = event code, three stack arguments (word 0 = EAX for the lookup 0xE660, word 1 = a quest-flag buffer). A code other than 0xFE returns EAX with its low byte cleared. For 0xFE: when the node's state byte (+0xC) is 1 and both 0xA568(buffer, 0x11, 0) and 0xA568(buffer, 0x11, 0xF) answer 0, returns 1. Otherwise the lookup 0xE660 (EAX = word 0, EBX = the node's +4 game, stack 0x20626262) is asked: a non-zero answer returns itself with the low byte set to 1, zero returns 0.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t fe53f0(GE& e, uint32_t eax, uint32_t node, uint32_t code, uint32_t s0, uint32_t s1, uint32_t)
{
    if (code != 0xfe) return eax & ~0xffu;
    if (e.rd8(node + 0xc) == 1 && e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x11, 0) == 0 && e.h(0xA568, 0, 0, 0, 0, 0, s1, 0x11, 0xf) == 0) return 1;
    const uint32_t r = e.h(0xE660, s0, 0, 0, e.rd32(node + 4), 0, 0x20626262);
    return r ? (r & ~0xffu) | 1 : 0;
}
}
