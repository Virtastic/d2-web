// 1.13c D2Multi+0x8F20 -- stdcall message handler: calls D2MCPClient 10050 and 10037; when 10037 answers non-zero, or 10054 answers zero, or the flag at 0x3A09C is set, it calls D2MCPClient 10019 with 0 and D2Win 10199. Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a09c;
namespace d2multi {
uint32_t f8f20(GE& e, uint32_t a0)
{
    (void)a0;
    e.h(0x7e12);
    if (e.h(0x7e5a) != 0 || e.h(0x7e2a) == 0 || g_d2multi_3a09c != 0) {
        e.h(0x7e84, 0, 0, 0, 0, 0, 0);
        e.h(0x7ca4);
    }
    return 1;
}
}
