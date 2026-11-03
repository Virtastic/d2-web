// 1.13c D2Game+0x7A020: queue message type 0x23 (16 bytes). ESI = owner, stack (ret 0xC) = b (byte to +0xE), w (word to +0xC), d (dword to +8).
#include "sunitmsg.h"
using namespace sunitmsg;
// 1.13c D2Game+0x7A020
extern "C" void d2game_7a020(uint32_t esi, uint32_t b, uint32_t w, uint32_t d, const SUnitMsgSeams* s) {
    const uint32_t m = alloc(*s, esi, 0x10, 0x32f);
    w8(m, 0xe, b); w8(m, 4, 0x23); w32(m, 0, 0); w16(m, 0xc, w); w32(m, 8, d);
    enqueue(*s, esi, m);
}
