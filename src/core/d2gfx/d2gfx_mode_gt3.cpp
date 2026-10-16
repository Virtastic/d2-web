#include <cstdint>
// 1.13c D2Gfx+0x0ADB0
//: returns 1 when the mode global at D2Gfx+0x11258 is above 3.
extern "C"
uint32_t D2Gfx_ADB0_ModeAbove3(int32_t mode) { return 3 < mode; }
