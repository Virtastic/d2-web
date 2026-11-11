// 1.13c D2Client+0x41F40 -- login packet copy. ECX = packet. When the two bytes at +0x33 are not "0\0",
// SStrCopy (Storm #501, stdcall: dest, src, 0x7FFFFFFF) copies that string to the global buffer at +F4320 (the callee gets the buffer address); then +0x1B to the buffer at +119940;
// then dword +0x15 -> 11C394, word +0x19 -> 119880, dword +0x225 -> 119924. The copies go through the host seam (the destinations are module globals).
#include <cstdint>
#include <emscripten.h>
#include "client_host.h"
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_41f40(D2cHost h, const uint8_t* pkt, uint32_t dstF4320, uint32_t dst119940, uint32_t* g11c394, uint16_t* g119880, uint32_t* g119924) {
    auto copy = [&](uint32_t dest, const uint8_t* src) { D2cCall c = {}; c.rva = 0xBE96; c.s[0] = dest; c.s[1] = (uint32_t)(uintptr_t)src; c.s[2] = 0x7FFFFFFFu; h(&c); };
    if (!(pkt[0x33] == '0' && pkt[0x34] == 0)) copy(dstF4320, pkt + 0x33);
    copy(dst119940, pkt + 0x1B);
    *g11c394 = *(const uint32_t*)(pkt + 0x15);
    *g119880 = *(const uint16_t*)(pkt + 0x19);
    *g119924 = *(const uint32_t*)(pkt + 0x225);
}
