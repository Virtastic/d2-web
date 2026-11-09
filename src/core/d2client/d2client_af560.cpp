// 1.13c D2Client+0xAF560: unit-queue handler (ECX = unit, EDX = packet); hands {b7, d8} to 0x2A750 with kind byte p[6].
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_unit_ports.h"
extern "C" {
// 1.13c D2Client+0xAF560
EMSCRIPTEN_KEEPALIVE void d2c_af560(const D2cPktEnv* e, uintptr_t unit, const uint8_t* p) {
    uint32_t l[7] = { p[7], 0, 0, 0, 0, 0, 0 };
    std::memcpy(&l[1], p + 8, 4);
    e->f2A750(p[6], unit, l);
}
}
