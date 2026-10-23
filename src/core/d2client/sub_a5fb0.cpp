// D2Client 0xA5FB0 env-seam implementation from the decompile.
// Register input in_EAX is an explicit parameter; callees are Env function pointers.
#include <cstdint>
namespace d2client_sub_a5fb0 {
using P = uintptr_t;
struct Env {
 uint32_t (*rd32)(P);
 void (*fada0b0)();                                   // FUN_6fada0b0 (D2Client+0x2A0B0)
 uint32_t (*o10265)(uint32_t); void (*o10024)(const char*, uint32_t);
 P (*o10392)(uint32_t);
 uint32_t (*f1630)(P); uint32_t (*f1660)(P);
 void (*o10739)(uint32_t, uint32_t, uint32_t, uint32_t);
};
// 1.13c D2Client+0xA5FB0  (a = in_EAX, retaddr = unaff_retaddr). Returns 2 on the fatal path
// (the caller's _exit(-1) is the seam's job), else 0.
uint32_t sub_A5FB0(const Env& e, P a, uint32_t retaddr) {
 e.fada0b0();
 uint32_t k = e.rd32(a + 0x2c);
 if (k == 0) { e.o10024("", e.o10265(0x946)); return 2; }
 P u = e.o10392(k);
 if (u && u != a) {
  if (e.rd32(a + 0x2c) == 0) { e.o10024("", e.o10265(0x7e2)); return 2; }
  uint32_t y = e.f1660(u), x = e.f1630(u);
  e.o10739(k, x, y, retaddr);
 }
 return 0;
}
}
