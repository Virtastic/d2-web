// 1.13c D2Client+0xBF990 drawNpcGoldOverlay: when bit 2 of the byte at +0xE264C is set and the object at +0x11BBFC exists, 0x9D630(EAX = obj, EDX = obj[0xC]) gives a record whose int at +0x18
// is printed in decimal (wsprintfA "%d", widened by win2Unicode) with D2Win #10150 at x 0x258 (stack 0x19, 4, 0). Both Unicode helpers are pure; the text is built natively.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_bf990(D2cHost h, uint32_t flags, const uint8_t* obj) {
    if (!(flags & 2) || !obj) return;
    D2cCall c = {}; c.rva = 0x9D630; c.eax = (uint32_t)(uintptr_t)obj; c.edx = *(const uint32_t*)(obj + 0xc);
    const uint8_t* r = (const uint8_t*)(uintptr_t)h(&c);
    if (!r) return;
    int32_t v = *(const int32_t*)(r + 0x18);
    uint16_t w[12] = {};
    char t[16]; int n = 0; uint32_t u = v < 0 ? 0u - (uint32_t)v : (uint32_t)v;
    do { t[n++] = (char)('0' + u % 10); u /= 10; } while (u);
    if (v < 0) t[n++] = '-';
    for (int i = 0; i < n && i < 11; i++) w[i] = (uint8_t)t[n - 1 - i];
    c = {}; c.rva = 0xD37E; c.ecx = (uint32_t)(uintptr_t)w; c.edx = 0x258; c.s[0] = 0x19; c.s[1] = 4; c.s[2] = 0; h(&c);
}
