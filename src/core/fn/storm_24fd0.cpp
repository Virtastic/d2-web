// storm+0x24fd0 cmd_table_free: FUNCTION-mode probe.
// The port is (src/core/storm/storm_batch_b51.cpp:Storm_Sub24FD0); the probe calls it, never reimplements it.
#include <emscripten.h>
#include <cstdint>

uint32_t Storm_Sub24FD0();

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_24fd0(uint32_t* r) {
    r[4] = Storm_Sub24FD0();
}
