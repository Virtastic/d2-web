// 1.13c D2Multi+0x8AC0 -- stdcall message handler: reads the text of the edit box at 0x3A030 (D2Win 10166); when there is one, converts it to a 0x1F-byte narrow buffer (D2Lang unicode2Win) and takes "non-empty" as the flag; then enables or disables the button at 0x3A028 with that flag (D2Win 10147). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a030;
__attribute__((weak)) uint32_t g_d2multi_3a028;
namespace d2multi {
uint32_t f8ac0(GE& e, uint32_t a0)
{
    (void)a0;
    const uint32_t t = e.h(0x7c50, 0, g_d2multi_3a030);
    uint32_t flag = 0;
    if (t) {
        uint8_t buf[0x20] = {};
        e.h(0x16040, 0, e.stk(buf), t, 0, 0, 0x1f);
        flag = buf[0] != 0;
    }
    e.h(0x7c14, 0, g_d2multi_3a028, flag);
    return 1;
}
}
