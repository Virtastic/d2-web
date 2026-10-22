// D2Common 1.13c 0x105C0: first list entry whose flag bit1 is clear gets a new type-2 room
// linked in; the room is returned. Register input (EAX=head) is explicit; retail callees are seams.
#include <cstdint>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
struct Cb {  // 94bd0, 5eec0, 94f90, 94950, 5e490
  uint32_t (*alloc)(int32_t);
  int32_t (*attach)(uint32_t, uint32_t);
  void (*release)(uint32_t);
  void (*noop)();
  void (*post)(int32_t);
};
}

extern "C" {
// 1.13c D2Common+0x105C0
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_105c0(uint32_t head, uint32_t a, uint32_t b, const Cb* c) {
  uint32_t t = 0;
  for (uint32_t e = w(head, 0x10); e; e = w(e, 0x24)) {
    if (w(w(e, 0x20), 0xC) >> 1 & 1) continue;
    t = c->alloc(2);
    const uint32_t src = w(w(t, 0x58), 0x14);
    w(t, 0x3C) = w(src, 0x10); w(t, 0x40) = w(src, 0x14);
    if (!c->attach(e, t)) { c->release(t); t = 0; }
    else {
      c->noop(); c->noop();
      const uint32_t o = w(e, 0x58);
      w(t, 0x24) = w(o, 0x10); w(o, 0x10) = t; w(o, 8) += 1;
      c->post(1);
      const uint32_t r = w(t, 0x20);
      w(r, 4) = b; w(r, 0) = a; w(r, 0xC) |= 2;
    }
    if (t) return t;
  }
  return t;
}
}
