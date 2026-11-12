// 1.13c D2Common+0x1C460 -- ECX = text, EDX = base, four stack arguments (the first = index). A null base, null text or empty text leaves EAX alone. Otherwise the worker 0x1BA60 runs with EBX = base + index * 0x54 + 0x4C and the text on the stack, and its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2c {
uint32_t f1c460(GE& e, uint32_t eax, uint32_t s, uint32_t p, uint32_t a1, uint32_t, uint32_t, uint32_t)
{
    if (!p || !s || !e.rd8(s)) return eax;
    return e.h(0x1BA60, 0, 0, 0, a1 * 0x54 + p + 0x4c, 0, s);
}
}
