// D2Common 1.13c batch: CRT x87 control word -> abstract control word (retail takes the hardware
// word in BX; here an explicit parameter).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x87B09 __abstract_cw
EMSCRIPTEN_KEEPALIVE uint32_t d2common_abstract_cw(uint16_t bx) {
 uint32_t r = 0;
 if (bx & 1) r = 0x10;
 if (bx & 4) r |= 8;
 if (bx & 8) r |= 4;
 if (bx & 0x10) r |= 2;
 if (bx & 0x20) r |= 1;
 if (bx & 2) r |= 0x80000;
 switch (bx & 0xc00) {
  case 0x400: r |= 0x100; break;
  case 0x800: r |= 0x200; break;
  case 0xc00: r |= 0x300; break;
 }
 if ((bx & 0x300) == 0) r |= 0x20000;
 else if ((bx & 0x300) == 0x200) r |= 0x10000;
 if (bx & 0x1000) r |= 0x40000;
 return r;
}
}
