// 1.13c Fog+0x06A63: CRT __doserrno(): returns &_getptd()->_tdoserrno. Wasm is single-threaded,
// so the per-thread CRT block collapses to one static.
#include <cstdint>

extern "C" uint32_t* fog_sub_6a63() {  // 1.13c Fog+0x06A63
    static uint32_t tdoserrno;
    return &tdoserrno;
}
