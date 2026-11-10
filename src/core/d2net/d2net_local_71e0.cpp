// 1.13c D2Net+0x71E0 (ordinal 10020), live form: the client takes one system packet (0xaf..0xb4).
// Returns its size, or -1 when the system list is empty.
#include "d2net_local.h"
#include <emscripten.h>

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2net_local_take_system(uint8_t* dst, uint32_t len) {
    return d2net_local_6ac0(&g_d2netLocal.q.second, dst, len);
}
