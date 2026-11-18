#include <cstdint>
// 1.13c D2Gfx+0x0B8A0
//: scale factor global at 0x6FA90BD8, step up by 0x20, ceiling 0x200.
extern "C"
uint32_t D2Gfx_B8A0_IncreaseScale(uint32_t x) {
  if ((int32_t)x < 0x200) { x += 0x20; if ((int32_t)x > 0x200) x = 0x200; }
  return x;
}
