// 1.13c D2Client+0x75780 (300 B), stdcall(name, y, colour, narrow) ret 0x10: the party-row player name. The narrow name is widened into a
// 100-char stack buffer (Unicode::toUnicode, n 100); when its width (D2Win 0xD35A) is over the limit (0x5F narrow, 0x87 wide) it is measured
// again and, while still over, the buffer is cut from char 98 down one char per measure; any cut name gets "..." (toUnicode n 5, strcat).
// Drawn by 0xD37E: ECX = buffer, EDX = gB9A0 + 0x24, stack (y - gB9A4 - 2, colour, 0). Returns the draw call's EAX.
#include <cstdint>
#include <emscripten.h>
#include "d2c_tounicode.h"

struct D2c75780Host {
    int32_t (*width)(const uint16_t* s);                                                 // 0xD35A: ECX = string
    uint32_t (*draw)(const uint16_t* s, int32_t x, int32_t y, uint32_t col, uint32_t z);  // 0xD37E: ECX, EDX, 3 stack words
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_75780(const char* name, int32_t y, uint32_t col, uint32_t narrow, int32_t g_b9a0, int32_t g_b9a4,
                         const D2c75780Host* h) {
    uint16_t buf[100] = {}, dots[5] = {};
    d2c::toUnicode(buf, name, 100);
    const int32_t lim = narrow ? 0x5f : 0x87;
    if (h->width(buf) > lim) {
        if (h->width(buf) > lim) {
            uint16_t* p = buf + 99;
            do { *--p = 0; } while (h->width(buf) > lim);
        }
        d2c::toUnicode(dots, "...", 5);
        d2c::wcat16(buf, dots);
    }
    return h->draw(buf, (int32_t)((uint32_t)g_b9a0 + 0x24u), (int32_t)((uint32_t)y - (uint32_t)g_b9a4 - 2u), col, 0);
}
