// 1.13c D2Client+0xAE3F0 (43 B, EAX = unit or null, no result): looks up id = dword +0xC (masked 0x7F by the callee's
// bucket math) in row kind = byte +0 of the table at module RVA 0x10A608 + kind * 0x200 through 0xA4E20; a hit goes to 0x82FE0.
// The row is handed to the seam as its module RVA.
#include <emscripten.h>
#include <cstdint>
extern "C" void d2c_port_ae3f0(uint32_t unit, uint32_t (*f4e20)(uint32_t key7, uint32_t rowRva, uint32_t kind), void (*f2fe0)(uint32_t hit)) {
    if (!unit) return;
    uint32_t kind = *(const uint32_t*)(uintptr_t)unit & 0xFF, id = *(const uint32_t*)(uintptr_t)(unit + 0xC);
    uint32_t hit = f4e20(id & 0x7F, 0x10A608u + kind * 0x200u, kind);
    if (hit) f2fe0(hit);
}
