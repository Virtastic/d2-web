// 1.13c D2Common+0x1C490 -- ECX = text, EDX = base, four stack arguments (the first = index). A null base, null text or empty text returns the text pointer. Otherwise the worker 0x1C020 runs with EAX = text, ECX = index and EBX = base + index * 8 + 0x14, and its result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2c {
uint32_t f1c490(GE& e, uint32_t s, uint32_t p, uint32_t a1, uint32_t, uint32_t, uint32_t)
{
    if (!p || !s || !e.rd8(s)) return s;
    return e.h(0x1C020, s, a1, 0, p + a1 * 8 + 0x14, 0);
}
}
