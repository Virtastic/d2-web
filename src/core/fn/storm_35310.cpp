// storm+0x35310 UTF-8 encode: FUNCTION-mode probe. The port is storm_batch_z11.cpp:Storm_35310; the probe calls it.
// Retail's function is void: its EAX at return is residue, so the descriptor masks the result to 0 (retmask 0).
// The probe writes that masked value; the encoded bytes at the buffer are the compared output.
#include <emscripten.h>
#include <cstdint>

void Storm_35310(uint32_t c, uint8_t* p);

extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_35310(uint32_t* r) {
    Storm_35310(r[0], (uint8_t*)(uintptr_t)r[1]);
    r[4] = 0;
}
