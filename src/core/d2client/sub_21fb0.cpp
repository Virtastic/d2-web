#include <stdint.h>
// 1.13c D2Client+0x21FB0
extern "C" void d2_client_21fb0(uint8_t* obj, uint32_t v) {
  if (!obj) __builtin_trap();  // retail: error message + _exit(-1)
  *(uint32_t*)(obj + 0x84) = v;
}
