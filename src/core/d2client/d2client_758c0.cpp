// 1.13c D2Client+0x758C0 (236 B): the party-row level label. EAX = row y, stack (level, colour), ret 8.
// Text = strncpy(str 0xD28, 20) + str 0xF9D + str 0xF9B + win2Unicode(wsprintfA("%2u", level), 9), each string through D2Lang 0xD1EC
// (ECX = id); drawn by 0xD37E: ECX = buffer, EDX = gB9A0 + 0x10A, stack (y - gB9A4, colour, 0). Returns the draw call's EAX.
#include <cstdint>
#include <emscripten.h>
#include "d2c_tounicode.h"

struct D2c758c0Host {
    const uint16_t* (*str)(uint32_t id);                                                  // 0xD1EC
    uint32_t (*draw)(const uint16_t* s, int32_t x, int32_t y, uint32_t col, uint32_t z);   // 0xD37E
};

extern "C" EMSCRIPTEN_KEEPALIVE
uint32_t d2_client_758c0(int32_t y, uint32_t level, uint32_t col, int32_t g_b9a0, int32_t g_b9a4, const D2c758c0Host* h) {
    uint16_t buf[100] = {}, num[10] = {};
    char a[16];
    d2c::wncpy16(buf, h->str(0xd28), 0x14);
    d2c::wcat16(buf, h->str(0xf9d));
    d2c::wcat16(buf, h->str(0xf9b));
    {   // wsprintfA(a, "%2u", level)
        char t[12]; int m = 0; uint32_t u = level;
        do { t[m++] = (char)('0' + u % 10); u /= 10; } while (u);
        int n = 0; if (m < 2) a[n++] = ' ';
        while (m) a[n++] = t[--m];
        a[n] = 0;
    }
    d2c::win2Unicode(num, a, 9);
    d2c::wcat16(buf, num);
    return h->draw(buf, (int32_t)((uint32_t)g_b9a0 + 0x10au), (int32_t)((uint32_t)y - (uint32_t)g_b9a4), col, 0);
}
