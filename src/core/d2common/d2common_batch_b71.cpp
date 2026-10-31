// D2Common 1.13c implementation batch b71. Register inputs are explicit parameters;
// unresolved retail callees are callback seams.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <emscripten.h>

extern "C" void d2_fn_d2common_e490(uint32_t* r);  // src/core/fn/d2common_e490.cpp

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }

struct Cb {  // 44bd0 AllocRoomEx, 0eec0 LinkMazeRooms (kind in EAX, e in ECX, trial in EDX), 44f90 FreeRoomEx
  uint32_t (*alloc)(uint32_t, int32_t);  // (level in EBX, type)
  int32_t (*attach)(int32_t, uint32_t, uint32_t);
  void (*release)(uint32_t);
};

// 1.13c D2Common+0x44950 on retail-layout blobs (ESI = owner, EAX = room, stack = direction): add a
// link node (0x18 bytes: room +0, direction +4, init +0xC, box +0x10 = room+0x34, next +0x14) to the
// owner's list unless the room is already in it. The wasm64 d2_fn_d2common_44950 takes native structs;
// these blobs cannot be passed to it.
void link44950(uint32_t owner, uint32_t room, uint32_t dir) {
  for (uint32_t n = w(owner, 0); n; n = w(n, 0x14)) if (w(n, 0) == room) return;
  const uint32_t n = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(std::calloc(0x18, 1)));
  w(n, 0x14) = w(owner, 0); w(owner, 0) = n;
  w(n, 0) = room; w(n, 4) = dir; w(n, 0xC) = 1; w(n, 0x10) = room + 0x34;
}

// Shared body of 0x10730 / 0x10820 / 0x10A00: pick the list entry with the best key at
// word keyIdx (greater if wantMax, else smaller) that lacks a child of the given kind.
uint32_t pick(uint32_t head, int keyIdx, bool wantMax, int32_t kind, uint32_t dirT, uint32_t dirE, const Cb* c) {
  uint32_t best = 0;
  for (uint32_t e = w(head, 0x10); e; e = w(e, 0x24)) {
    const int32_t k = static_cast<int32_t>(w(e, keyIdx * 4));
    const bool better = !best || (wantMax ? static_cast<int32_t>(w(best, keyIdx * 4)) < k
                                          : k < static_cast<int32_t>(w(best, keyIdx * 4)));
    if (!better || (w(w(e, 0x20), 0xC) >> 1 & 1)) continue;
    bool found = false;
    for (uint32_t n = w(e, 0); n; n = w(n, 0x14)) if (static_cast<int32_t>(w(n, 4)) == kind) { found = true; break; }
    if (found) continue;
    uint32_t t = c->alloc(w(e, 0x58), 2);
    const uint32_t src = w(w(t, 0x58), 0x14);
    w(t, 0x3C) = w(src, 0x10); w(t, 0x40) = w(src, 0x14);
    if (!c->attach(kind, e, t)) { c->release(t); t = 0; }
    else {
      link44950(e, t, dirT); link44950(t, e, dirE);
      const uint32_t o = w(e, 0x58);
      w(t, 0x24) = w(o, 0x10); w(o, 0x10) = t; w(o, 8) += 1;
      uint32_t r[5] = {t, 1, 0, 0, 0};  // retail EBX = the trial room
      d2_fn_d2common_e490(r);
    }
    if (t) { c->release(t); best = e; }
  }
  return best;
}
}

extern "C" {
// 1.13c D2Common+0x106B0
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_106b0(uint32_t rec, uint32_t list, uint32_t* counter, void (*add)(uint32_t, uint32_t, uint32_t)) {
  const uint32_t v = w(rec, 8);
  for (uint32_t n = w(list, 0x10); n; n = w(n, 0x24)) {
    const uint32_t o = w(n, 0x20);
    if (!(w(o, 0xC) >> 1 & 1) && w(o, 0) == w(rec, 0)) {
      w(o, 0) = w(rec, 4); w(o, 4) = v; w(o, 0xC) |= 2;
      goto done;
    }
  }
  add(w(rec, 0xC), w(rec, 4), v);
done:
  if (counter) {
    uint32_t u = (*counter + 1) & 0x80000003u;
    if (static_cast<int32_t>(u) < 0) u = ((u - 1) | 0xFFFFFFFCu) + 1;
    *counter = u;
  }
}
// 1.13c D2Common+0x10730
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_10730(uint32_t head, const Cb* c) { return pick(head, 0xE, true, 3, 3, 1, c); }
// 1.13c D2Common+0x10820
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_10820(uint32_t head, const Cb* c) { return pick(head, 0xD, true, 2, 2, 0, c); }
// 1.13c D2Common+0x10A00
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_10a00(uint32_t head, const Cb* c) { return pick(head, 0xE, false, 1, 1, 3, c); }

// 1.13c D2Common+0x1A080
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_1a080(uint32_t u, void (*a)(), void (*b)(uint32_t), void (*c)()) {
  if (!(w(u, 0x28) & 0x1000000)) a();
  if (!(w(u, 0x28) & 0x2000000) && w(u, 0x48) == 2) b(u);
  if (!(w(u, 0x28) & 0x100000)) c();
  return w(u, 0x30);
}

// 1.13c D2Common+0x19B60: pick a random matching entry (LCG seed at +0x1C4/+0x1C8).
// tbl is the 8-byte-stride table at DAT_6fde4f48.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_19b60(uint32_t self, int32_t id, int32_t* o1, int32_t* o2, const uint32_t* tbl, void (*fin)(int32_t, int32_t, int32_t, uint32_t)) {
  const int32_t n = static_cast<int32_t>(w(self, 0x1D8));
  auto match = [&](int32_t v) { return id == v || (tbl[id * 2] != 0 && tbl[id * 2 + 1] == tbl[v * 2 + 1]); };
  uint32_t cnt = 0;
  for (int32_t i = 0; i < n; i++) if (match(static_cast<int32_t>(w(self, 0x34 + i * 12)))) cnt++;
  int32_t pick = 0;
  if (cnt) {
    uint64_t s = static_cast<uint64_t>(w(self, 0x1C4)) * 0x6AC690C5ull + w(self, 0x1C8);
    *reinterpret_cast<uint64_t*>(p(self + 0x1C4)) = s;
    pick = (cnt & (cnt - 1)) == 0 ? static_cast<int32_t>(((cnt - 1) & static_cast<uint32_t>(s)) + 1)
                                  : static_cast<int32_t>(static_cast<uint32_t>(s) % cnt + 1);
  }
  int32_t i = 0;
  for (; i < n; i++) {
    if (match(static_cast<int32_t>(w(self, 0x34 + i * 12)))) pick--;
    if (pick < 1) break;
  }
  *o1 = static_cast<int32_t>(w(self, 0x2C + i * 12));
  *o2 = static_cast<int32_t>(w(self, 0x30 + i * 12));
  fin(*o1, *o2, static_cast<int32_t>(w(self, 0x1B4)), self);
}
}
