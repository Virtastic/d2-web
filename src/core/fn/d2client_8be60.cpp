// 1.13c D2Client+0x8be60 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_8be60(uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8be60(uint32_t* r) { r[4] = d2_client_8be60(*(const uint32_t*)(uintptr_t)r[0]); }
