// 1.13c D2Client+0x6D220 (76 B, EAX = from, ECX = to, ESI = dword*): rescale a packed field through
// Ordinal_10057 (thunk 0xC10C, args (value, from, to, 0)).
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_6d220(uint32_t from, uint32_t to, uint32_t* p, uint32_t (*o10057)(uint32_t v, uint32_t from, uint32_t to, uint32_t z)) {
    if (from == 8) {
        if (to == 4) *p = ((o10057((*p << 3) >> 6, 8, to, 0) << 6) >> 3);
    } else if (from == 0x10 && to == 8) {
        *p = ((o10057((*p << 4) >> 6, 0x10, to, 0) << 6) >> 4);
    }
}
