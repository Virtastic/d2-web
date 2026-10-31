#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x2FD10 (EAX passes to 0x17F0, stack = idx): the u16 at record+8+idx*2 for the record 0x17F0
// returns, or 0x1506 when it returns 0. `lookup` is 0x17F0 (record address as 32-bit).
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_2fd10_fn(uint32_t eax, int32_t idx, uint32_t (*lookup)(uint32_t)) {
    uint32_t rec = lookup(eax);
    if (!rec) return 0x1506;
    uint16_t w;
    std::memcpy(&w, reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rec)) + 8 + idx * 2, 2);
    return w;
}
