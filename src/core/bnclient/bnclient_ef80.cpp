// 1.13c Bnclient+0xEF80 -- ECX, EDX, three stack arguments, ret 12; the third is a flag word (bit 0 expansion, bit 1 extra step). Calling it while the news reader is active (0x1F638) is fatal (line 0x2F). Picks the news tag at 0x1E0F0 ("BNNews-D2XP" with bit 0, "BNNews-D2DV" without), clears 0x1ED80 / 0x1EF00, calls 0xC520 (ECX value, EDX value); with bit 1 calls 0xC5F0 (ECX = the second stack argument, EDX = 1, stack = bit 0); waits on 0x10D90 with 10; then under the state lock at 0x1F1DC sets the state at 0x1F1F4: with the flag at 0x1F644 set, to 0xE only if it was 0; otherwise to 0. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f638;
__attribute__((weak)) uint32_t g_bnclient_1e0f0;
__attribute__((weak)) uint32_t g_bnclient_1ed80;
__attribute__((weak)) uint32_t g_bnclient_1ef00;
__attribute__((weak)) uint32_t g_bnclient_1f644;
__attribute__((weak)) uint32_t g_bnclient_1f1f4;
namespace bnclient {
uint32_t fef80(GE& e, uint32_t c, uint32_t d, uint32_t s0, uint32_t s1, uint32_t fl)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x80e4); e.h(0x80de, 0, 0, 0, 0, 0, 0xB201a813u, v, line); return e.h(0x1569, 0, 0, 0, 0, 0, 0xffffffffu); };
    (void)s0;
    const uint32_t xp = fl & 1, more = fl & 2;
    if (g_bnclient_1f638) fatal(0x2f);
    g_bnclient_1e0f0 = xp ? 0xB201a6e0u : 0xB201a6d4u;
    g_bnclient_1ed80 = 0;
    g_bnclient_1ef00 = 0;
    e.h(0xc520, 0, 0, 0, 0, 0, c, d);
    if (more) e.h(0xc5f0, 0, s1, 1, 0, 0, xp);
    e.h(0x10d90, 0, 0, 0, 0, 0, 10);
    e.h(0x191ac, 0, 0, 0, 0, 0, 0xB201f1dcu);
    if (g_bnclient_1f644) {
        if (!g_bnclient_1f1f4) g_bnclient_1f1f4 = 0xe;
    } else g_bnclient_1f1f4 = 0;
    e.h(0x191b0, 0, 0, 0, 0, 0, 0xB201f1dcu);
    return 1;
}
}
