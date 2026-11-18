// 1.13c D2Client+0xADF40: object assign (ECX = packet); forwards the fields to 0xA75C0 with ESI = p[1].
#include <cstdint>
#include <cstring>
#include <emscripten.h>
#include "d2client_unit_ports.h"
extern "C" {
// 1.13c D2Client+0xADF40
EMSCRIPTEN_KEEPALIVE void d2c_adf40(const D2cObjectEnv* e, const uint8_t* p) {
    uint32_t guid; uint16_t w6, w8, wA;
    std::memcpy(&guid, p + 2, 4); std::memcpy(&w6, p + 6, 2); std::memcpy(&w8, p + 8, 2); std::memcpy(&wA, p + 10, 2);
    e->fA75C0(p[1], guid, w6, w8, wA, 0, p[12], p[13]);
}
}
