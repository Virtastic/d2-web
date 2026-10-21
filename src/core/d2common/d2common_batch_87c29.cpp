// D2Common 1.13c batch: CRT _control87. Retail reads the x87 control word (fnstcw -> BX) and
// loads the result back (fldcw); here the current word is an explicit parameter and the new word
// is not stored.
#include <cstdint>
#include <emscripten.h>

extern "C" {
uint32_t __attribute__((weak)) d2common_abstract_cw(uint16_t bx);
uint32_t __attribute__((weak)) d2common_hw_cw(uint32_t bx);

// 1.13c D2Common+0x87C29 __control87
EMSCRIPTEN_KEEPALIVE uint32_t d2common_control87(uint32_t newValue, uint32_t mask, uint16_t hwCw) {
 const uint32_t old = d2common_abstract_cw(hwCw);
 const uint32_t res = old & ~mask | newValue & mask;
 d2common_hw_cw(res);
 return res;
}
}
