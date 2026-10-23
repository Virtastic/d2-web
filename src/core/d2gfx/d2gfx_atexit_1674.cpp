#include <cstdint>
#include <emscripten.h>

extern "C" uint32_t d2gfx_onexit(uint32_t func);

// 1.13c D2Gfx+0x01674 (18 B, _atexit): returns 0 when __onexit(func) is non-null, else -1.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2gfx_atexit(uint32_t func) { return (d2gfx_onexit(func) != 0) - 1; }
