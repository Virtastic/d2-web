// 1.13c D2Client+0xBE2E0 armTooltipTimer: while the gate at +0xDD5D4 is set, the deadline at +0xFAD50 := GetTickCount() + 100.
#include <cstdint>
#include <emscripten.h>
extern "C" EMSCRIPTEN_KEEPALIVE void d2c_be2e0(const uint32_t* gate, uint32_t* deadline, uint32_t (*tick)()) {
    if (*gate != 0) *deadline = tick() + 100;
}
