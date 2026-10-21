// D2Common 1.13c batch: room-pair bounding box, then hand to 0x70BB0.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Weak: wired from other branches/main. 0x3BA60 = room coords (8 dwords); 0x70BB0 consumes the box.
__attribute__((weak)) void d2_d2common_3ba60(uint32_t room, int32_t* out);
__attribute__((weak)) void d2_d2common_70bb0(int32_t h, const int32_t* box, uint32_t* p);

// 1.13c D2Common+0x70E40
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_70e40(uint32_t* p) {
 *(uint8_t*)&p[5] = (uint8_t)(p[5] << 1);
 int32_t b[8], a[8], c[8];
 d2_d2common_3ba60(p[2], b);
 for (int i = 0; i < 8; ++i) a[i] = b[i];
 const uint32_t other = p[3];
 if (other != 0 && p[2] != other) {
  d2_d2common_3ba60(other, c);
  if (c[0] < a[0]) { b[0] = c[0]; b[2] = (a[2] - c[0]) + a[0]; }
  else { b[0] = a[0]; b[2] = (c[2] - a[0]) + c[0]; }
  if (c[1] < a[1]) { b[3] = (a[3] - c[1]) + a[1]; b[1] = c[1]; }
  else { b[3] = (c[3] - a[1]) + c[1]; b[1] = a[1]; }
 }
 if (b[2] == a[2]) { b[0] -= 10; b[2] += 0x14; }
 if (b[3] == a[3]) { b[1] -= 10; b[3] += 0x14; }
 d2_d2common_70bb0(b[3], b, p);
}
}
