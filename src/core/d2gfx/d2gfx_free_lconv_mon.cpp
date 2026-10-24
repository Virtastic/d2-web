// 1.13c D2Gfx+0x0670C: VC7.1 CRT locale monetary-string cleanup.
// Module globals and the Win32 heap (inside d2gfx_free's env) are seams; fields stay 32-bit retail pointers.
#include <cstdint>
#include <emscripten.h>

struct D2GfxFreeEnv;
extern "C" void d2gfx_free(void* p, const D2GfxFreeEnv* e);  // D2Gfx+0x02BD4 (d2gfx_free_2bd4.cpp)

// lconv/default_lconv point at records; monetary fields are +0x0C..+0x24 (seven).
extern "C" EMSCRIPTEN_KEEPALIVE void d2gfx_free_lconv_mon(
    const uint32_t* lconv, const uint32_t* default_lconv,
    const uint32_t* static_mon, const D2GfxFreeEnv* free_env) {
    if (lconv == nullptr) return;
    for (int i = 0; i < 7; ++i) {
        const uint32_t field = lconv[3 + i];
        if (field != default_lconv[3 + i] && field != static_mon[i])
            d2gfx_free(reinterpret_cast<void*>(static_cast<uintptr_t>(field)), free_env);
    }
}
