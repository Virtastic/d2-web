// 1.13c BNClient+0x13390 -- drop the cache file: an open handle (DAT_1DCE8 != -1) is closed and the global set back to -1,
// the path builder at 0x12EE0 fills a 260-byte stack buffer (EAX = buffer, first byte cleared), and the file is deleted.
// Returns DeleteFileA's answer.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_bnclient_1dce8;
namespace bnclient {
uint32_t f13390(GE& e)
{
    if (g_bnclient_1dce8 != 0xFFFFFFFFu) {
        e.h(0x19174, 0, 0, 0, 0, 0, g_bnclient_1dce8);   // CloseHandle
        g_bnclient_1dce8 = 0xFFFFFFFFu;
    }
    char path[0x104];
    path[0] = 0;
    e.h(0x12EE0, e.stk(path));
    return e.h(0x1916C, 0, 0, 0, 0, 0, e.stk(path));     // DeleteFileA
}
}
