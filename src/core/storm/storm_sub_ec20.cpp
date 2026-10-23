// src/core/storm/storm_sub_ec20.cpp -- 1.13c Storm+0x0EC20
#include <cstdint>
#include <cstring>
// PKWare explode-state init. State is retail's 32-bit layout, addressed by byte offset.
struct StormExplodeEnv {
  uint32_t (*read)(uint8_t* buf, uint32_t* size, void* user); // the callback stored at +0x28
  void* user;                                                 // param_4, stored at +0x24
  void (*copy)(void* dst, const void* src, uint32_t dwords);  // F2A0 (count 0x100/0x10/0x20/0x40 as passed)
  void (*build)(uint32_t n, uint8_t* a, const uint8_t* b, uint8_t* c); // F160
  void (*decodeTbl)(uint8_t* st);                             // F1A0
  uint32_t (*fill)(uint8_t* st);                              // EDA0
  const uint8_t *t40d60, *t40d40, *t40d50, *t40d10, *t40d20, *t40c90, *t40cd0;
};
static inline uint32_t& W(uint8_t* s, uint32_t o) { return *(uint32_t*)(s + o); }
// 1.13c Storm+0x0EC20: returns 0 ok, 1 bad dict size, 2 bad type, 3 short input, 4 EDA0==0x306.
extern "C" uint8_t Storm_SubEC20(StormExplodeEnv& e, uint32_t cbLo, uint32_t cbHi, uint8_t* st, void* user) {
  e.user = user; W(st, 0x28) = cbLo; W(st, 0x2c) = cbHi; W(st, 0x24) = 0;
  uint32_t sz = 0x800; W(st, 0x1c) = sz;
  uint32_t n = e.read(st + 0x2234, &sz, e.user); W(st, 0x1c) = sz; W(st, 0x20) = n;
  if (n < 5) return 3;
  W(st, 4) = st[0x2234]; W(st, 0xc) = st[0x2235]; W(st, 0x14) = st[0x2236]; W(st, 0x18) = 0;
  W(st, 0x1c) = 3;
  uint32_t d = W(st, 0xc);
  if (d < 4 || d > 6) return 1;
  W(st, 0x10) = 0xffffu >> ((0x10 - d) & 0x1f);
  if (W(st, 4) != 0) {
    if (W(st, 4) != 1) return 2;
    e.copy(st + 0x2fb4, e.t40d60, 0x100);
    e.decodeTbl(st);
  }
  e.copy(st + 0x30f4, e.t40d40, 0x10);
  e.build(0x10, st + 0x30f4, e.t40d50, st + 0x2b34);
  e.copy(st + 0x3104, e.t40d10, 0x10);
  e.copy(st + 0x3114, e.t40d20, 0x20);
  e.copy(st + 0x30b4, e.t40c90, 0x40);
  e.build(0x40, st + 0x30b4, e.t40cd0, st + 0x2a34);
  return e.fill(st) == 0x306 ? 4 : 0;
}
