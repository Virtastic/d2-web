// 1.13c D2Multi+0xD000 -- stdcall, one ignored argument. When the D2Win import 0x7C62 (ECX = the control global at
// image offset 0x3A028) answers 0 it returns 0. Otherwise the edit box text (D2Win import 0x7C50, ECX = global 0x3A030),
// when there is one, is narrowed into a 256-byte stack buffer (Unicode::unicode2Win, import slot 0x16040: ECX = buffer,
// EDX = text, one stack word 0x100) and handed to 0xC690 (ECX = buffer). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2multi_3a028;
__attribute__((weak)) uint32_t g_d2multi_3a030;
namespace d2multi {
uint32_t fd000(GE& e, uint32_t)
{
    if (!e.h(0x7C62, 0, g_d2multi_3a028)) return 0;
    const uint32_t text = e.h(0x7C50, 0, g_d2multi_3a030);
    if (text) {
        char narrow[0x100];
        e.h(0x16040, 0, e.stk(narrow), text, 0, 0, 0x100);
        e.h(0xC690, 0, e.stk(narrow));
    }
    return 1;
}
}
