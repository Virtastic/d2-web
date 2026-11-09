// 1.13c D2Client+0xBE430 flushQueuedMessageText: draws the queued wide message at +0x11C980 (D2Win #10028 width, #10083 height, #10150 draw at a clamped x, y-2) and clears its 0x28 dwords.
// Unicode::strlen is a pure D2Lang import (done natively). msg = the 0x28-dword buffer, cca20/cca24 = the anchor words after it, scrW/scrH = +0xDBC48/+0xDBC4C.
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_be430(D2cHost h, uint16_t* msg, int32_t cca20, int32_t cca24, int32_t scrW, int32_t scrH) {
    if (msg[0] == 0) return;
    const uint32_t kMsg = 0x6fbcc980u;
    D2cCall c = {}; c.rva = 0xD35A; c.ecx = kMsg; int32_t esi = (int32_t)h(&c) + 8;        // D2Win #10028 (text width)
    c = {}; c.rva = 0xD38A; int32_t eax = (int32_t)h(&c) + 7;                              // D2Win #10083 (line height)
    int32_t edx = cca20 - (esi >> 1), ecx = cca24 + 2;
    if (edx < 0) edx = 0;
    if (edx + esi > scrW) edx = scrW - esi;
    if (ecx - eax < 0) ecx = eax;
    if (ecx > scrH - 0x1f) ecx = scrH - 0x1f;
    c = {}; c.rva = 0xD37E; c.ecx = kMsg; c.edx = (uint32_t)edx; c.s[0] = (uint32_t)(ecx - 2); c.s[1] = 0; c.s[2] = 1; h(&c);   // D2Win #10150
    for (int i = 0; i < 0x28; i++) ((uint32_t*)msg)[i] = 0;
}
