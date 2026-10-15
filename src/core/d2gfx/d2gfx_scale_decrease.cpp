#include <cstdint>
// 1.13c D2Gfx+0x0B870
//: scale factor global at 0x6FA90BD8, step down by 0x20, floor 0x100.
extern "C"
uint32_t D2Gfx_B870_DecreaseScale(uint32_t x) {
  if ((int32_t)x > 0x100) { x -= 0x20; if ((int32_t)x < 0x100) x = 0x100; }  // signed: retail leaves 0xFFFFFFFF alone
  return x;
}
