// storm+0x281A0 FN probe: stdcall (handle, signal) = r[0], r[1]; blob E (r[14]) = g_lastError, blob EV (r[16]) = the wake event.
#include <emscripten.h>
#include <cstdint>
extern "C" void storm_281a0_fnrun(uint32_t* gErr, uint32_t* gEv, int32_t h, int32_t sig);
extern "C" EMSCRIPTEN_KEEPALIVE void d2_fn_storm_281a0(uint32_t* r) {
    storm_281a0_fnrun((uint32_t*)(uintptr_t)r[14], (uint32_t*)(uintptr_t)r[16], (int32_t)r[0], (int32_t)r[1]);
    r[4] = 0;
}
