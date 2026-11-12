// 1.13c D2Multi+0x11400 -- stdcall message handler: reads the text of the edit box at 0x3A0D0 (D2Win 10166); when it is missing or empty (D2Lang Unicode::strlen) returns 1; otherwise converts it to a 0x100-byte narrow buffer (D2Lang unicode2Win) and returns what 0xD5B0 answers for that buffer (EAX = 0, EDI = the buffer).
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a0d0;
namespace d2multi {
uint32_t f11400(GE& e, uint32_t a0)
{
    (void)a0;
    const uint32_t t = e.h(0x7c50, 0, g_d2multi_3a0d0);
    if (!t || !e.h(0x1601c, 0, t)) return 1;
    uint8_t buf[0x100];
    e.h(0x16040, 0, e.stk(buf), t, 0, 0, 0x100);
    e.ed(e.stk(buf));
    const uint32_t r = e.h(0xd5b0, 0);
    e.edi(0xd5b0, e.stk(buf));
    return r;
}
}
