// 1.13c D2Client+0xBE8E0 -- stdcall message handler (one argument, the message). Walks the node list at 0x113AA4 (next pointer at +0x22) clearing the dword at +0x1E, marks the message handled (+0x18 = 1, +0x1C = 0) and passes it to Storm ordinal 511 (0xBE84), whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_113aa4;
static inline uint32_t n32(uint32_t a) { uint32_t v; __builtin_memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
namespace d2cl {
uint32_t fbe8e0(GE& e, uint32_t msg)
{
    for (uint32_t n = g_d2client_113aa4; n; n = n32(n + 0x22)) e.wr32(n + 0x1e, 0);
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
