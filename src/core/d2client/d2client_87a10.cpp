// 1.13c D2Client+0x87A10 -- stdcall, two arguments. Calls 0x87940 with EBX = argument 1 and one stack word, argument 2; the low byte of the result is 1 when it answers non-zero, 0 otherwise (the upper bytes are the answer's).
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace d2cl {
uint32_t f87a10(GE& e, uint32_t a, uint32_t b)
{
    const uint32_t r = e.h(0x87940, 0, 0, 0, a, 0, b);
    return (r & ~0xffu) | (r != 0);
}
}
