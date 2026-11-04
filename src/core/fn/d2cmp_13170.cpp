// d2cmp+0x13170 DccColorMapBuild: FUNCTION-mode probe.
// The port is src/core/d2cmp/d2cmp_13170.cpp; the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>
void d2cmp_13170(uint8_t* outA, const uint8_t* flags, uint8_t* outB);

// r[0] = flags (EDX), r[1] = outA (ESI), r[2] = outB (stack arg 2)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2cmp_13170(uint32_t* r) {
    d2cmp_13170((uint8_t*)(uintptr_t)r[1], (const uint8_t*)(uintptr_t)r[0], (uint8_t*)(uintptr_t)r[2]);
    r[4] = 0;
}
