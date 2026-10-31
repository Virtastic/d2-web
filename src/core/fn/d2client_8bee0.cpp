// 1.13c D2Client+0x8bee0 FN replay probe
#include <emscripten.h>
#include <cstdint>
#include <cstring>
extern "C" uint32_t d2_client_8bee0(uint32_t, uint32_t);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_d2client_8bee0(uint32_t* r) { const uint32_t* g = (const uint32_t*)(uintptr_t)r[0]; r[4] = d2_client_8bee0(g[0], g[1]); }
