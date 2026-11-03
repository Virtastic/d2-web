// 1.13c D2Game+0x7A440: queue message type 0x9A (20 bytes). ESI = owner, stack (ret 0x14) = w1 (word to +8), b1 (byte to +0xA), w2 (word to +0xC), w3 (word to +0xE), b2 (byte to +0x10).
#include "sunitmsg.h"
using namespace sunitmsg;
// 1.13c D2Game+0x7A440
extern "C" void d2game_7a440(uint32_t esi, uint32_t w1, uint32_t b1, uint32_t w2, uint32_t w3, uint32_t b2, const SUnitMsgSeams* s) {
    const uint32_t m = alloc(*s, esi, 0x14, 0x298);
    w16(m, 8, w1); w8(m, 0xa, b1); w16(m, 0xc, w2); w8(m, 4, 0x9a); w32(m, 0, 0); w16(m, 0xe, w3); w8(m, 0x10, b2);
    enqueue(*s, esi, m);
}
