// 1.13c D2Client+0xa4d40 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_a4d40(uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_a4d40(uint32_t* r) { r[4] = d2_client_a4d40(*(const uint32_t*)(uintptr_t)r[0]); }
