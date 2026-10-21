// D2Common 1.13c batch: MSVC CRT __fassign. Callees not yet wired are weak externs named by RVA.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Retail passes the _CRT_DOUBLE out-buffer in a register-ish stack slot; here explicit (out, number).
__attribute__((weak)) void D2Common_878D6(uint8_t out[8], const uint8_t* number);  // string -> double
__attribute__((weak)) void D2Common_8791B(uint8_t out[8], const uint8_t* number);  // string -> float (4 bytes used)

// 1.13c D2Common+0x871CB: store parsed number into *argument as double (flag!=0) or float.
// Oddity: in the float path retail reuses the `flag` stack slot as the output buffer and copies 4 bytes.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_871cb(int32_t flag, uint8_t* argument, const uint8_t* number) {
 uint8_t buf[8] = {0};
 if (flag != 0) {
  D2Common_878D6(buf, number);
  for (int i = 0; i < 8; ++i) argument[i] = buf[i];
  return;
 }
 D2Common_8791B(buf, number);
 for (int i = 0; i < 4; ++i) argument[i] = buf[i];
}
}
