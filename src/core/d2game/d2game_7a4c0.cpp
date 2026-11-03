// 1.13c D2Game+0x7A4C0: queue message type 0x99 (20 bytes). ESI = owner, stack (ret 0x14) = w (word to +8), b1 (byte to +0xA), b2 (byte to +0xB), d (dword to +0xC), b3 (byte to +0x10).
#include "sunitmsg.h"
using namespace sunitmsg;
// 1.13c D2Game+0x7A4C0
extern "C" void d2game_7a4c0(uint32_t esi, uint32_t w, uint32_t b1, uint32_t b2, uint32_t d, uint32_t b3, const SUnitMsgSeams* s) {
    const uint32_t m = alloc(*s, esi, 0x14, 0x281);
    w16(m, 8, w); w8(m, 0xa, b1); w8(m, 0xb, b2); w8(m, 4, 0x99); w32(m, 0, 0); w32(m, 0xc, d); w8(m, 0x10, b3);
    enqueue(*s, esi, m);
}
