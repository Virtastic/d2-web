// 1.13c D2Client+0x8be10 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_8be10(uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8be10(uint32_t* r) { r[4] = d2_client_8be10(*(const uint32_t*)(uintptr_t)r[0]); }
