// Leaf rows b13: pinned-global / blob-arg only.
#include <emscripten.h>
#include <cstdint>

// 1.13c D2Client+0x8C65: a = [0xFA8A4], b = [0xFA8B0]; a==2 && b >= 5 (unsigned) -> 1, else 3.
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Client_HeapSelect8C65(uint32_t a, uint32_t b) { return (a == 2 && b >= 5) ? 1u : 3u; }

// 1.13c D2Client+0x016A0: EAX = pUnit; type 0 with a pointer at +0x14 -> zero the four dwords at that struct's +0x150.
extern "C" EMSCRIPTEN_KEEPALIVE void D2Client_Clear16A0(uint8_t* unit) {
    if (!unit || *(uint32_t*)unit != 0) return;
    uint32_t p = *(uint32_t*)(unit + 0x14);
    if (!p) return;
    uint32_t* d = (uint32_t*)(uintptr_t)p;
    d[0x54] = d[0x55] = d[0x56] = d[0x57] = 0;
}
