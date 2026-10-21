// D2Common 1.13c batch: CRT abstract control word -> x87 control word (retail takes it in EBX;
// here an explicit parameter).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x87B9B __hw_cw
EMSCRIPTEN_KEEPALIVE uint32_t d2common_hw_cw(uint32_t bx) {
 uint32_t r = (bx & 0x10) != 0;
 if (bx & 8) r |= 4;
 if (bx & 4) r |= 8;
 if (bx & 2) r |= 0x10;
 if (bx & 1) r |= 0x20;
 if (bx & 0x80000) r |= 2;
 switch (bx & 0x300) {
  case 0x100: r |= 0x400; break;
  case 0x200: r |= 0x800; break;
  case 0x300: r |= 0xc00; break;
 }
 if ((bx & 0x30000) == 0) r |= 0x300;
 else if ((bx & 0x30000) == 0x10000) r |= 0x200;
 if (bx & 0x40000) r |= 0x1000;
 return r;
}
}
