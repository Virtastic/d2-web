#include <cstdint>
#include <emscripten.h>

extern "C" void d2gfx_doexit(uint32_t code, uint32_t quick, uint32_t retcaller);

// 1.13c D2Gfx+0x0150B (17 B, _exit): doexit(code, 0, 0). Void.
extern "C" EMSCRIPTEN_KEEPALIVE __attribute__((weak)) void d2gfx_exit(int32_t code) { d2gfx_doexit((uint32_t)code, 0, 0); }
