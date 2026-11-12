// 1.13c Bnclient+0xE9D0 -- ECX = out pointer (NULL is fatal, line 0x280): stores the dword at 0x1F1F8 through it and returns that dword.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1f1f8;
namespace bnclient {
uint32_t fe9d0(GE& e, uint32_t p)
{
    auto fatal = [&](uint32_t line) { const uint32_t v = e.h(0x80e4); e.h(0x80de, 0, 0, 0, 0, 0, 0xB201a813u, v, line); return e.h(0x1569, 0, 0, 0, 0, 0, 0xffffffffu); };
    if (!p) { fatal(0x280); return g_bnclient_1f1f8; }
    e.wr32(p, g_bnclient_1f1f8);
    return g_bnclient_1f1f8;
}
}
