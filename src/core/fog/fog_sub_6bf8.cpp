// 1.13c Fog+0x06BF8: _unlock(1) -- CRT lock-table leave on entry 1 (sole caller 0x6ADF).
// Fog+0x3C3F (_unlock, LeaveCriticalSection) is a still-UNMAPPED row; host seam, no-op when absent.
#include <cstdint>
#include <emscripten.h>

extern "C" __attribute__((weak)) void fog_unlock_3c3f(int32_t index);

extern "C" EMSCRIPTEN_KEEPALIVE void fog_sub_6bf8(void) {  // 1.13c Fog+0x06BF8
    if (fog_unlock_3c3f) fog_unlock_3c3f(1);
}
