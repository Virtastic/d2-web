// 1.13c D2Game+0x7A090: queue message type 0xA4 (12 bytes). ESI = owner, stack (ret 4) = w (word to +8). Tail-jumps to the signal thunk with ESI as its word.
#include "sunitmsg.h"
using namespace sunitmsg;
// 1.13c D2Game+0x7A090
extern "C" void d2game_7a090(uint32_t esi, uint32_t w, const SUnitMsgSeams* s) {
    const uint32_t m = alloc(*s, esi, 0xc, 0x31f);
    w8(m, 4, 0xa4); w32(m, 0, 0); w16(m, 8, w);
    enqueue(*s, esi, m);
}
