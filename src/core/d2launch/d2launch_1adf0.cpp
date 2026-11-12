// 1.13c D2Launch+0x1ADF0 -- stdcall, one ignored argument: the TCP/IP join screen's OK button. The edit box text (D2Win
// import 0x98C2, ECX = the control global at image offset 0x25E44) is narrowed into a 1024-byte stack buffer
// (Unicode::toUtf, import slot 0x1C034: ECX = buffer, EDX = text, stack word 0x100). An empty text returns at once.
// Otherwise the text is kept in the 256-byte global at 0x25AE0 (strncpy of 0xFF, last byte cleared) and saved to the
// registry (Storm import 0x9796: key "Diablo II" 0x1E204, value name 0x1DC74, flags 0, the global). When the text is not
// four dotted numbers (sscanf 0x1073 answering other than 4) the Fog resolver 0x97D8 (ECX = buffer) turns the name into an
// address string that replaces the buffer; a null answer puts the error text 0x1E256 into the edit box (0x98BC) and
// returns. The address goes to the launch block (global 0x25ABC) at +0x33, the screen is torn down (0x14C60), the
// connect callback 0x11B70 is installed (0x99A0), the result global 0x2550C is cleared and D2Net connects (0x9A8A: stack
// words 0 and the buffer); then the callback 0x11B30 is installed. A result still 0 closes the connection (0x9A96),
// 0x12B60 runs and the error box (0x15090) shows string 0x145B with the handler 0x17B30. A non-zero result runs 0x12B60,
// 0x14C60, installs 0x13CE0, closes (0x9A96), runs 0x12B60 again, and when global 0x25E60 is 0 marks the launch block
// (state global 0x25AD4 := 2, +0x15 := 9, word +0x19 := 1) and leaves the screen (0x9A5A, 0x15D30, 0xD5D0). Returns 1.
#include <cstdint>
#include "../fn/d2game_gp.h"
__attribute__((weak)) uint32_t g_d2launch_25e44;
__attribute__((weak)) uint32_t g_d2launch_25abc;
__attribute__((weak)) uint32_t g_d2launch_25ad4;
__attribute__((weak)) uint32_t g_d2launch_2550c;
__attribute__((weak)) uint32_t g_d2launch_25e60;
__attribute__((weak)) uint8_t g_d2launch_25ae0_text[256];
namespace d2launch {
uint32_t f1adf0(GE& e, uint32_t)
{
    const uint32_t M = 0xB2000000u;
    uint32_t q[4];
    char buf[0x400];
    const uint32_t text = e.h(0x98C2, 0, g_d2launch_25e44);
    e.h(0x1C034, 0, e.stk(buf), text, 0, 0, 0x100);
    if (buf[0] == 0) return 1;
    {
        uint32_t i = 0;
        for (; i < 0xFF && buf[i]; i++) g_d2launch_25ae0_text[i] = (uint8_t)buf[i];
        for (; i < 0xFF; i++) g_d2launch_25ae0_text[i] = 0;
        g_d2launch_25ae0_text[0xFF] = 0;
    }
    e.h(0x9796, 0, 0, 0, 0, 0, M | 0x1E204, M | 0x1DC74, 0, M | 0x25AE0);
    if (e.h(0x1073, 0, 0, 0, 0, 0, e.stk(buf), M | 0x1E1F8, e.stk(&q[1]), e.stk(&q[3])) != 4) {
        const uint32_t name = e.h(0x97D8, 0, e.stk(buf));
        if (!name) {
            e.h(0x98BC, 0, g_d2launch_25e44, M | 0x1E256);
            return 1;
        }
        uint32_t i = 0;
        uint8_t c;
        do { c = GE::rd8(name + i); buf[i] = (char)c; i++; } while (c);
    }
    {
        uint32_t i = 0;
        char c;
        do { c = buf[i]; GE::wr8(g_d2launch_25abc + 0x33 + i, (uint8_t)c); i++; } while (c);
    }
    e.h(0x14C60);
    e.h(0x99A0, 0, 0, 0, 0, 0, M | 0x11B70);
    g_d2launch_2550c = 0;
    e.h(0x9A8A, 0, 0, 0, 0, 0, 0, e.stk(buf));
    e.h(0x99A0, 0, 0, 0, 0, 0, M | 0x11B30);
    if (g_d2launch_2550c == 0) {
        e.h(0x9A96);
        e.h(0x12B60);
        const uint32_t s = e.h(0x9B32, 0, 0x145B);
        e.h(0x15090, 0, 0, 0, 0, 0, s, M | 0x17B30, 1);
        return 1;
    }
    e.h(0x12B60);
    e.h(0x14C60);
    e.h(0x99A0, 0, 0, 0, 0, 0, M | 0x13CE0);
    e.h(0x9A96);
    e.h(0x12B60);
    if (g_d2launch_25e60 == 0) {
        g_d2launch_25ad4 = 2;
        GE::wr32(g_d2launch_25abc + 0x15, 9);
        GE::wr8(g_d2launch_25abc + 0x19, 1);
        GE::wr8(g_d2launch_25abc + 0x1A, 0);
        e.h(0x9A5A);
        e.h(0x15D30);
        e.h(0xD5D0);
    }
    return 1;
}
}
