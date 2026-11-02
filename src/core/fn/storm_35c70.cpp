// storm+0x35c70 Storm_35C70: FUNCTION-mode probe.
// Register order per the descriptor (regs ecx edx esi): r[0]=ECX (unused), r[1]=EDX (seq), r[2]=ESI (key).
// The port is (src/core/storm/storm_batch_18f90.cpp:Storm_Sub35C70); the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>

extern "C" uint32_t* Storm_Sub35C70(uint32_t key, uint32_t seq);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_35c70(uint32_t* r) {
    r[4] = (uint32_t)(uintptr_t)Storm_Sub35C70(r[2], r[1]);
}
