// 1.13c D2Client+0xA4A90 over the FnCallees table. EAX = a, ECX = b, EDI = target; plain ret. 0 when a == b; otherwise walks the
// list at a+0x60 (first 0xC4F0, next 0xC568, item 0xC526; each stdcall one word) and returns 1 when an item is the target.
// Fatal 0xA5B on a null item.
#include "d2client_batch_a3040.h"
namespace d2client_batch_a3040 {
uint32_t sub_A4A90(FnCallees& s, uint32_t a, uint32_t b, uint32_t target) {
 if (b == a) return 0;
 const uint32_t l = fnAt<uint32_t>(fnPtr(a), 0x60);
 if (!l) return 0;
 for (uint32_t i = s.h(0xc4f0, 0, 0, 0, 0, 0, l); i; i = s.h(0xc568, 0, 0, 0, 0, 0, i)) {
  const uint32_t r = s.h(0xc526, 0, 0, 0, 0, 0, i);
  if (!r) s.fatal(0xa5b);
  if (r == target) return 1;
 }
 return 0;
}
}
