// 1.13c Bnclient+0xB890 -- ECX = count, EDX = id, stack (dest pointer array, size array), ret 8: copies the strings of the pending reply at 0x1F890 into the caller buffers. Returns 0 when the client is not up (0x1EDF0), when there is no reply, or when its kind (+0, must be 1), count (+4) or id (+8) do not match. Otherwise walks the packed strings at +0xC: a non-empty one goes through the bounded copy 0x1AF0 (dest, string, size) and the byte at dest + size is zeroed, an empty one zeroes the first dest byte; Storm 506 gives the length to step over. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1edf0;
__attribute__((weak)) uint32_t g_bnclient_1f890;
namespace bnclient {
uint32_t fb890(GE& e, uint32_t n, uint32_t id, uint32_t dst, uint32_t sz)
{
    if (!g_bnclient_1edf0) return 0;
    const uint32_t p = g_bnclient_1f890;
    if (!p || e.rd32(p) != 1 || e.rd32(p + 4) != n || e.rd32(p + 8) != id) return 0;
    uint32_t s = p + 0xc;
    for (uint32_t i = 0; i < n; i++) {
        const uint32_t d = e.rd32(dst + 4 * i);
        if (e.rd8(s)) {
            e.h(0x1af0, 0, 0, 0, 0, 0, d, s, e.rd32(sz + 4 * i));
            e.wr8(e.rd32(sz + 4 * i) + e.rd32(dst + 4 * i), 0);
        } else e.wr8(d, 0);
        s += e.h(0x8042, 0, 0, 0, 0, 0, s) + 1;
    }
    return 1;
}
}
