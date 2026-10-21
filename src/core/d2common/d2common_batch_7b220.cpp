// D2Common 1.13c batch. Retail's hidden ESI is the hash-table base (128 bucket heads at +0,
// 128 at +0x200, pending-list head at +0x400); h1 = DAT_6fddb458, h2 = DAT_6fddb658 (128 ints each).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x7B220: pop the pending-list head, unlink it from its bucket, push on the second bucket set.
// Ghidra types the callee void, but 0x7B630 consumes EAX as the popped node (null when the list is empty).
EMSCRIPTEN_KEEPALIVE intptr_t d2_fn_d2common_7b220(uint32_t* t, const int32_t* h1, const int32_t* h2) {
 auto P = [](uint32_t v) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(v)); };
 auto W = [](uint8_t* n) { return reinterpret_cast<uint32_t*>(n + 0x30); };
 uint8_t* n = P(t[0x100]);
 if (!n) return 0;
 t[0x100] = *reinterpret_cast<uint32_t*>(n + 0x34);
 uint32_t k = (h2[n[2] & 0x7f] + h1[n[0] & 0x7f]) & 0x7f;
 uint8_t* c = P(t[k]);
 if (c != n) {
  uint8_t* prev;
  do { prev = c; c = P(*W(prev)); } while (c != n);
  *W(prev) = *W(n);
 } else t[k] = *W(n);
 *W(n) = 0;
 k = (h2[n[2] & 0x7f] + h1[n[0] & 0x7f]) & 0x7f;
 *W(n) = t[0x80 + k];
 t[0x80 + k] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(n));
 return reinterpret_cast<intptr_t>(n);
}
}
