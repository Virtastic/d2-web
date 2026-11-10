// 1.13c D2Client+0xA3AD0 over the FnCallees table. No arguments; plain ret. With a menu active (0x11BC04): 0x3CC60 (EAX = 0xE) with
// 0xD0B4(text 0xDE858); when 0x42090 holds, 0x3CC60 (EAX = 1) with 0xD0B4(text 0xDE900); the open count 0x10B208 drops by one.
// Then frees the menu list from 0x11BC00 (next at +0xB0C) through 0xBF86 (ECX = node, EDX = file 0xD41D4, 0x336, 0) and clears it.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
void sub_A3AD0(FnCallees& s) {
 if (*s.g(0x11bc04)) {
  uint32_t t = s.h(0xd0b4, 0, 0, 0, 0, 0, s.mod(0xde858));
  s.h(0x3cc60, 0xe, 0, 0, 0, 0, t, s.mod(0xde858));
  if (s.h(0x42090)) {
   t = s.h(0xd0b4, 0, 0, 0, 0, 0, s.mod(0xde900));
   s.h(0x3cc60, 1, 0, 0, 0, 0, t, s.mod(0xde900));
  }
  --*s.g(0x10b208);
 }
 for (uint32_t n = *s.g(0x11bc00); n;) {
  const uint32_t nx = fnAt<uint32_t>(fnPtr(n), 0xb0c);
  s.h(0xbf86, 0, n, s.mod(0xd41d4), 0, 0, 0x336, 0);
  n = nx;
 }
 *s.g(0x11bc00) = 0;
}
}
