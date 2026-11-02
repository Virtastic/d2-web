// storm+0x2adc0 bitset find from the top: FUNCTION-mode probe. The port is storm_batch_2ad10.cpp:Storm_2ADC0; the probe calls it.
#include <emscripten.h>
#include <cstdint>
#include "../storm/storm_big_types.h"

uint32_t Storm_2ADC0(const Storm2fVector* value);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_2adc0(uint32_t* r) {
    r[4] = Storm_2ADC0((const Storm2fVector*)(uintptr_t)r[0]);
}
