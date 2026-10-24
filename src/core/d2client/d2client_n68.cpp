// d2client_n68: faithful ports; DLL globals -> state struct, imports/callees -> host seams.
#include <cstdint>
#include <cstddef>
#include <cstdlib>

// 1.13c D2Client+0x01000
extern "C" void d2c_1000_delete(void* p, void (*smemfree)(void*, const char*, int32_t, int32_t)) {
  if (p != nullptr) smemfree(p, "delete", -1, 0);
}

// 1.13c D2Client+0x10000
extern "C" void* d2c_10000_zcalloc(void* (*crt_calloc)(uint32_t, uint32_t), uint32_t items, uint32_t size) {
  return crt_calloc(items, size);
}

// seams for D2Client+0x4DC30
struct D2c4DC30Seams {
  void (*a5fb0)();            // 0xA5FB0
  void (*ord10770)(int32_t);  // Ordinal_10770
  void (*f2a6c0)();           // 0x2A6C0
};
// seam variant of D2Client+0x4DC30
extern "C" void d2c_4dc30_seams(const int32_t* unit, const D2c4DC30Seams* h) {
  if (unit != nullptr && unit[0] == 1) {
    uint32_t m = (uint32_t)unit[4];
    if (m < 0x10 && m != 0 && m != 0xc) {
      h->a5fb0();
      h->ord10770(unit[0xb]);
      h->f2a6c0();
    }
  }
}

// 1.13c D2Client+0x1BF20
struct D2c1BF20State { int32_t gate_ada4; int32_t flag_c4d8; int32_t ctx_bbfc; int32_t sub_c2f4, sub_c2f8; };
struct D2c1BF20Seams {
  void (*f5f0)();                       // 0xEF5F0 (reset)
  const int32_t* (*f1a80)();            // 0x1A80 (current unit)
  int32_t (*f1630)(const int32_t*);
  int32_t (*f1660)(const int32_t*);
  int32_t (*ord10973)(int32_t, int, int);
  void (*f1e30)();
  int32_t (*cc0)(uint32_t*, int32_t*);  // 0x1BCC0 / BC40 / BDE0 / B620 / BBC0 / 8A00 by index 1,3,0,2,4,5
  int32_t (*c40)(uint32_t*, int32_t*);
  int32_t (*de0)(uint32_t*, int32_t*);
  int32_t (*b620)(uint32_t*, int32_t*);
  int32_t (*bc0)(uint32_t*, int32_t*);
  int32_t (*a00)(uint32_t*, int32_t*);
};
extern "C" int32_t d2c_1bf20(D2c1BF20State* s, const D2c1BF20Seams* h, int32_t kind, int32_t x, int32_t y, uint8_t fl) {
  if (!(s->gate_ada4 == 0 && (s->flag_c4d8 == 0 || (x == 0 && y == 0)))) return 0;
  s->flag_c4d8 = 1;
  h->f5f0();
  int32_t ctx = s->ctx_bbfc;
  uint32_t flags = 0, bits = 0;
  const int32_t* u = h->f1a80();
  int32_t pos[2] = {x, y};
  if (u && (*u == 2 || *u == 4)) { pos[0] = h->f1630(u); pos[1] = h->f1660(u); }
  if ((fl & 8) && h->ord10973(ctx, 10, 0) != 0) bits = 0x40;
  if (fl & 4) bits |= 0x20;
  switch (kind) {
    case 0: if (u) h->f1e30(); flags = bits | 5; if (!u) s->sub_c2f4 = s->sub_c2f8 = 0; return h->de0(&flags, pos);
    case 1: flags = bits | 9; return h->cc0(&flags, pos);
    case 2: flags = bits | 0x11; s->sub_c2f4 = s->sub_c2f8 = 0; return h->b620(&flags, pos);
    case 3: if (u) h->f1e30(); else s->sub_c2f4 = s->sub_c2f8 = 0; flags = bits | 6; return h->c40(&flags, pos);
    case 4: flags = bits | 10; return h->bc0(&flags, pos);
    case 5: flags = bits | 0x12; s->sub_c2f4 = s->sub_c2f8 = 0; return h->a00(&flags, pos);
    default: return 1;
  }
}
