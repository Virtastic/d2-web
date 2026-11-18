#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x1FD00 (EAX = unit, stack = mask): true when the unit is non-null, its type (dword 0) is 1,
// the pointer at +0x14 is non-null and the u16 at that pointer +0x16 shares a bit with the mask.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_1fd00(const uint8_t* unit, uint32_t mask) {
    if (!unit) return 0;
    uint32_t type, p;
    std::memcpy(&type, unit, 4);
    std::memcpy(&p, unit + 0x14, 4);
    if (type != 1 || !p) return 0;
    uint16_t w;
    std::memcpy(&w, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(p)) + 0x16, 2);
    return (w & (mask & 0xFFFFu)) != 0;
}
