// 1.13c D2Client+0xA3A20 over the FnCallees table. No arguments; plain ret. 0x398C0 (EDX = 0x26, ESI = 1, preserves EDX) gates a
// scan of the 114 ten-byte records at 0x11A2E0 {i32 key, u16 value @+4, i32 flag @+6} for key 0x26 flag 1: a value below 0xE0
// (and not 0xFFFF; the scan key is EDX after the call) goes to 0xDE85C. Then 0x398C0 (EDX = 0x26, ESI = 0) gates the same scan for flag 0 into 0xDE868.
#include <cstring>
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
static void scanA3A20(FnCallees& s, uint32_t key, int32_t flag, uint32_t dst) {
 const uint8_t* t = (const uint8_t*)s.g(0x11a2e0);
 for (uint32_t i = 0; i < 114; ++i) {
  int32_t k, f; uint16_t v;
  memcpy(&k, t + i * 10, 4); memcpy(&v, t + i * 10 + 4, 2); memcpy(&f, t + i * 10 + 6, 4);
  if ((uint32_t)k == key && f == flag) { if (v != 0xffff && v < 0xe0) *s.g(dst) = v; return; }
 }
}
void sub_A3A20(FnCallees& s) {
 if (s.h(0x398c0, 0, 0, 0x26, 0, 1)) scanA3A20(s, s.edxAfter(0x26), 1, 0xde85c);
 if (s.h(0x398c0, 0, 0, 0x26, 0, 0)) scanA3A20(s, s.edxAfter(0x26), 0, 0xde868);
}
}
