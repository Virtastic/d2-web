// 1.13c Bnclient+0xBC20 -- ECX = text: a NULL text or one longer than 0xFF characters (Storm 506) is fatal (Fog 10265 / 10024 with the line, then the exit helper); otherwise sends frame 0x59 with the text and its terminator as the payload. Returns 1 when the send answers zero or more, 0 otherwise.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace bnclient {
uint32_t fbc20(GE& e, uint32_t s)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x80e4); e.h(0x80de, 0, 0, 0, 0, 0, 0xB201a813u, v, line); return e.h(0x1569, 0, 0, 0, 0, 0, 0xffffffffu); };
    uint32_t n;
    if (!s) n = fatal(0x58d);
    else {
        n = e.h(0x8042, 0, 0, 0, 0, 0, s) + 1;
        if (n > 0x100) n = fatal(0x593);
    }
    const uint32_t r = e.h(0xeb00, 0, n, 0, 0, 0, 0x59);
    if (s) e.edi9(0xeb00, (const void*)(uintptr_t)s, 0xFF);
    return (int32_t)r >= 0;
}
}
