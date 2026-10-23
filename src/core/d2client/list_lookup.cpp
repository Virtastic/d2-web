// Linked-list lookups on the list headed at D2Client+0x11C4D4: node +8 = key, +0x30 = next.
#include <emscripten.h>
#include <cstdint>
#include <cstring>
static uint32_t rd(const uint8_t* p, uint32_t o) { uint32_t v; memcpy(&v, p + o, 4); return v; }
// 1.13c D2Client+0x21670: node +4 of the first node whose key matches, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_21670(uint32_t key, const uint8_t* n) {
    for (; n; n = (const uint8_t*)(uintptr_t)rd(n, 0x30)) if (rd(n, 8) == key) return rd(n, 4);
    return 0;
}
// 1.13c D2Client+0x216D0: node +0 of the first node whose key matches, else 0.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_216d0(uint32_t key, const uint8_t* n) {
    for (; n; n = (const uint8_t*)(uintptr_t)rd(n, 0x30)) if (rd(n, 8) == key) return rd(n, 0);
    return 0;
}
