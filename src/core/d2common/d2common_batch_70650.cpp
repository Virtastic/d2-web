// D2Common 1.13c batch. Retail structs keep 32-bit layout: node +8/+10 u16 x/y, +0x14 next link;
// owner +0x30 -> record whose +0x9c holds the output point dwords.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x70650: walk the node chain, keep a point where the step direction changes
// (max 0x4d), emit them reversed (last kept first) into owner->+0x30->+0x9c; return count or 0.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_70650(const uint8_t* owner, const uint8_t* node) {
 uint32_t out[0x4e];
 uint8_t* dst = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(owner + 0x30)) + 0x9c);
 uint32_t n = 0;
 int32_t px = -2, py = -2;
 while (node) {
  if (n > 0x4d) break;
  const uint8_t* nx = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(*reinterpret_cast<const uint32_t*>(node + 0x14)));
  if (nx) {
   const int32_t dx = *reinterpret_cast<const uint16_t*>(node + 8) - *reinterpret_cast<const uint16_t*>(nx + 8);
   const int32_t dy = *reinterpret_cast<const uint16_t*>(node + 10) - *reinterpret_cast<const uint16_t*>(nx + 10);
   if (dx != px || dy != py) {
    out[0x4d - n] = *reinterpret_cast<const uint32_t*>(node + 8);
    ++n; px = dx; py = dy;
   }
  }
  node = nx;
 }
 if (n < 2 || n > 0x4d) return 0;
 for (uint32_t i = 0; i < n; ++i) reinterpret_cast<uint32_t*>(dst)[i] = out[0x4e - n + i];
 return n;
}
}
