// 1.13c D2Client+0x42600 -- stdcall message handler. With the active flag 0x11C3A0 clear nothing happens and 0 is returned. Otherwise a set callback pointer 0x11A764 is checked with IsBadCodePtr (retail halts on a bad one) and called. The message is marked handled (+0x18 = 1, +0x1C = 0) and passed to Storm ordinal 511, whose result is returned.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2client_11c3a0;
__attribute__((weak)) uint32_t g_d2client_11a764;
namespace d2cl {
uint32_t f42600(GE& e, uint32_t msg)
{
    if (!g_d2client_11c3a0) return 0;
    if (g_d2client_11a764) e.h(HookEnv::handlerRva(g_d2client_11a764));
    e.wr32(msg + 0x18, 1);
    e.wr32(msg + 0x1c, 0);
    return e.h(0xBE84, 0, 0, 0, 0, 0, msg);
}
}
