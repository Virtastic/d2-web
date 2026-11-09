// 1.13c D2Client+0xBF370 (EBX = wide string, stack font/y/colour, ret 0xC): draws the string centred in the free band of the screen. The band is computed by 0xBEA10
// (open-panel flags at +0xFAD80..+0xFAE14 halve the left or right edge of the screen width at +0xDBC48), then D2Win #10184 (font), #10028 (width), #10150 (draw), #10184 (restore).
// act = the dword table at +0xFAD80, w = screen width. Unicode::strcpy and the 256-char ctor loop are pure D2Lang imports, done natively.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
static int32_t half(int32_t v) { return (v - (v >> 31)) >> 1; }
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_bf370(D2cHost h, const uint16_t* str, uint32_t a1, uint32_t a2, uint32_t a3, const uint32_t* act, int32_t w) {
    static const uint8_t kZero[6] = { 0x5c, 0x64, 0x68, 0x30, 0x70, 0x6c };
    static const uint8_t kL[7] = { 0x74, 0x58, 0x08, 0x3c, 0x90, 0x94, 0x50 };
    static const uint8_t kR[2] = { 0x04, 0x10 };
    uint16_t buf[0x100] = {};
    for (uint8_t o : kZero) if (act[o / 4]) return;
    int32_t o0 = 0, o2 = w;
    for (uint8_t o : kL) if (act[o / 4]) { o0 = half(w); break; }
    for (uint8_t o : kR) if (act[o / 4]) { o2 = half(w); break; }
    if (o0 == o2 || !str) return;
    D2cCall c = {}; c.rva = 0xD3A8; c.ecx = a1; uint32_t esi = h(&c);                       // D2Win #10184
    for (uint32_t i = 0; ; i++) { buf[i] = str[i]; if (!str[i]) break; }
    c = {}; c.rva = 0xD35A; c.ecx = (uint32_t)(uintptr_t)buf; int32_t wl = (int32_t)h(&c);  // D2Win #10028
    c = {}; c.rva = 0xD37E; c.ecx = (uint32_t)(uintptr_t)buf; c.edx = (uint32_t)(((o0 + o2) >> 1) - (wl >> 1)); c.s[0] = a2; c.s[1] = a3; c.s[2] = 1; h(&c);   // D2Win #10150
    c = {}; c.rva = 0xD3A8; c.ecx = esi; h(&c);
}
