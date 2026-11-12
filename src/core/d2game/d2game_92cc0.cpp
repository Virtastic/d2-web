// 1.13c D2Game+0x92CC0 -- ECX = a three-word record {first, target, -, source}, EDX = argument: 0x70 bytes of the +0xC source are copied to the stack and the helper 0xA10B0 runs (EAX = the +4 word, ESI = the copy, stack = the +0 word and the argument). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2g {
uint32_t f92cc0(GE& e, uint32_t p, uint32_t x)
{
    e.h(0xA10B0, e.rd32(p + 4), 0, 0, 0, 0xB3000000u, e.rd32(p), x);
    return 1;
}
}
