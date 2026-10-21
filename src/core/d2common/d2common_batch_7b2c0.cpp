// D2Common 1.13c batch. a = EAX struct (retail 32-bit layout, +8 and +0x30 are pointers).
#include <cstdint>
#include <emscripten.h>

extern "C" {
}
extern "C" uint32_t d2_chk_4ce90(intptr_t room, uint32_t x, uint32_t y, uint32_t pattern, uint32_t mask)
 __attribute__((weak));

extern "C" {
// 1.13c D2Common+0x7B2C0
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_7b2c0(const int32_t* a) {
 const int32_t* p = *reinterpret_cast<const int32_t* const*>(a + 12); // +0x30, 32-bit ptr in retail
 if (p[0x58 / 4] == 0) return 1;
 const uint32_t x = *reinterpret_cast<const uint16_t*>(reinterpret_cast<const char*>(a) + 4);
 const uint32_t y = *reinterpret_cast<const uint16_t*>(reinterpret_cast<const char*>(a) + 6);
 const intptr_t room = a[2];
 const uint32_t pat = static_cast<uint32_t>(a[10]), mask = static_cast<uint32_t>(a[11]);
 static const int8_t o[8][2] = {{-2,-2},{-2,2},{2,-2},{2,2},{-2,0},{0,-2},{2,0},{0,2}};
 for (int i = 0; i < 8; ++i)
  if (d2_chk_4ce90(room, x + o[i][0], y + o[i][1], pat, mask) == 0) return 1;
 return 0;
}
}
