// 1.13c D2Net+0x7200 (ordinal 10008), live form: the client takes one game packet (opcode <= 0xae).
// Returns its size, or -1 when the game list is empty.
#include "d2net_local.h"
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_take(uint8_t* dst, uint32_t len) {
    return d2net_local_6ac0(&g_d2netLocal.q.normal, dst, len);
}
