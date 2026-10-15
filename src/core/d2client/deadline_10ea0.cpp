#include <cstdint>
#include <emscripten.h>

namespace { uint32_t g_deadline = 0; }  // D2Client+0x11C554

// 1.13c D2Client+0x10EA0 (tick = GetTickCount, supplied by the platform layer)
extern "C" EMSCRIPTEN_KEEPALIVE void d2_client_10ea0(uint32_t tick) { g_deadline = tick + 500u; }
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t d2_client_10ea0_get() { return g_deadline; }
