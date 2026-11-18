#include <emscripten.h>
#include <cstdint>
#include <cstring>

// 1.13c D2Client+0x1D8F0 (ECX = arg): runs 0x1CA00(arg) when the byte at 0x123395 is under 5 (signed), clears
// 0x1223F0, 0x1223D8 and the byte 0x1223F3, then runs 0x1CAD0. Globals and callees are passed in.
struct D2c1D8F0Env { const int32_t* g123395; uint8_t* g1223f0; uint32_t* g1223d8; uint8_t* g1223f3; void (*f1ca00)(uint32_t); void (*f1cad0)(); };
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_1d8f0(uint32_t arg, const D2c1D8F0Env* e) {
    if (*e->g123395 < 5) e->f1ca00(arg);
    *e->g1223f0 = 0; *e->g1223d8 = 0; *e->g1223f3 = 0;
    e->f1cad0();
}
