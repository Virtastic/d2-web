// 1.13c Bnclient+0xC770 -- ECX = value: clears the two answer slots at 0x1EE5C and 0x1EFDC and sends frame 0x41 with the value as its 4-byte payload; a negative send returns 0. Otherwise waits for the answer (0x10D90 with 0x41) and returns the reply at 0x1F894 plus 4, or 0 when there is none.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1ee5c;
__attribute__((weak)) uint32_t g_bnclient_1efdc;
__attribute__((weak)) uint32_t g_bnclient_1f894;
namespace bnclient {
uint32_t fc770(GE& e, uint32_t v)
{
    g_bnclient_1ee5c = 0;
    g_bnclient_1efdc = 0;
    const uint32_t r = e.h(0xeb00, 0, 4, 0, 0, 0, 0x41);
    e.edi9(0xeb00, &v, 0xF);
    if ((int32_t)r < 0) return 0;
    e.h(0x10d90, 0, 0, 0, 0, 0, 0x41);
    return g_bnclient_1f894 ? g_bnclient_1f894 + 4 : 0;
}
}
