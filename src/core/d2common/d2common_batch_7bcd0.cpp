// D2Common 1.13c batch. Retail struct layout (32-bit fields, offsets in bytes); links are
// retail 32-bit pointers, so the walk is done with intptr_t arithmetic on byte offsets.
#include <cstdint>
#include <emscripten.h>

static inline int32_t& F(intptr_t p, int off) { return *reinterpret_cast<int32_t*>(p + off); }

extern "C" {
// 1.13c D2Common+0x7BCD0: unlink node (EAX) from its current list, push it on the head of the
// list in (node->+0x58)->+0x1b4. Oddity: node->+0x1c is set to list+0x18c (a head sentinel),
// +0xe8 to the old head (list+0x274); a node already flagged (+0x44 == 1) is left alone.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7bcd0(intptr_t n) {
 if (*reinterpret_cast<char*>(n + 0x44) == 1) return;
 if (F(n, 0xE8) != 0 && F(n, 0x1C) != 0) {
  F(F(n, 0xE8), 0x1C) = F(n, 0x1C);
  F(F(n, 0x1C), 0xE8) = F(n, 0xE8);
  F(n, 0xE8) = 0;
  F(n, 0x1C) = 0;
 }
 const intptr_t l = F(F(n, 0x58), 0x1B4);
 F(n, 0x1C) = static_cast<int32_t>(l + 0x18C);
 F(n, 0xE8) = F(l, 0x274);
 F(F(l, 0x274), 0x1C) = static_cast<int32_t>(n);
 F(l, 0x274) = static_cast<int32_t>(n);
 *reinterpret_cast<char*>(n + 0x44) = 1;
}
}
