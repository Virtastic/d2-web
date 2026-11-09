// 1.13c D2Client+0xBF5F0 (EDI = wide string s, one stack byte digit, ret 4, returns s): a nonempty s becomes D2Lang string 0xF9A + the character (digit + '0') + s.
// The Unicode helpers (ctor, strcpy, strcat, win2Unicode) are pure D2Lang imports, done natively; wsprintfA("%c") is the character.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
static uint32_t wlen(const uint16_t* s) { uint32_t n = 0; while (s[n]) n++; return n; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2c_bf5f0(D2cHost h, uint16_t* s, uint32_t digit) {
    if (!s[0]) return (uint32_t)(uintptr_t)s;
    uint16_t buf[1024] = {};
    D2cCall c = {}; c.rva = 0xD1EC; c.ecx = 0xf9a;
    const uint16_t* pre = (const uint16_t*)(uintptr_t)h(&c);
    uint32_t n = 0; while (pre[n]) { buf[n] = pre[n]; n++; }
    uint8_t ch = (uint8_t)((digit & 0xff) + '0');
    if (ch) buf[n++] = ch;
    for (uint32_t i = 0, m = wlen(s); i <= m; i++) buf[n + i] = s[i];
    for (uint32_t i = 0; ; i++) { s[i] = buf[i]; if (!buf[i]) break; }
    return (uint32_t)(uintptr_t)s;
}
