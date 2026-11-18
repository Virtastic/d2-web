#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x21580 (ECX = key): walks the list headed at DAT_6FBCC4D4 (next at +0x30) for the node whose
// dword at +8 equals the key and returns its dword at +0x1C, or 0. Retail pointers are 32-bit words.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_21580(uint32_t head, uint32_t key) {
    while (head) {
        const uint8_t* n = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(head));
        uint32_t k;
        std::memcpy(&k, n + 8, 4);
        if (k == key) { uint32_t v; std::memcpy(&v, n + 0x1C, 4); return v; }
        std::memcpy(&head, n + 0x30, 4);
    }
    return 0;
}
