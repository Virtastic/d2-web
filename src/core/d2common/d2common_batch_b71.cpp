// D2Common 1.13c implementation batch b71. Register inputs are explicit parameters;
// unresolved retail callees are callback seams.
#include <cstdint>
#include <cstdlib>
#include <initializer_list>
#include <emscripten.h>

extern "C" void d2common_maze_pick_room_preset_blob(uint32_t* r);  // d2common_maze_pick_blob.cpp

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }

struct Cb {  // 44bd0 AllocRoomEx, 0eec0 LinkMazeRooms (kind in EAX, e in ECX, trial in EDX), 44f90 FreeRoomEx
  uint32_t (*alloc)(uint32_t, int32_t);  // (level in EBX, type)
  int32_t (*attach)(int32_t, uint32_t, uint32_t);
  void (*release)(uint32_t);
};

// D2Common 0x44950 on retail-layout blobs (ESI = owner, EAX = room, stack = direction): add a
// link node (0x18 bytes: room +0, direction +4, init +0xC, box +0x10 = room+0x34, next +0x14) to the
// owner's list unless the room is already in it. The wasm64 D2Common_44950_Engine takes native structs;
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
      d2common_maze_pick_room_preset_blob(r);
    }
    if (t) { c->release(t); best = e; }
  }
  return best;
}
}

extern "C" {
// 1.13c D2Common+0x106B0
EMSCRIPTEN_KEEPALIVE void d2common_106b0_engine(uint32_t rec, uint32_t list, uint32_t* counter, void (*add)(uint32_t, uint32_t, uint32_t)) {
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
EMSCRIPTEN_KEEPALIVE uint32_t d2common_10730_engine(uint32_t head, const Cb* c) { return pick(head, 0xE, true, 3, 3, 1, c); }
// 1.13c D2Common+0x10820
EMSCRIPTEN_KEEPALIVE uint32_t d2common_10820_engine(uint32_t head, const Cb* c) { return pick(head, 0xD, true, 2, 2, 0, c); }
// 1.13c D2Common+0x10A00
EMSCRIPTEN_KEEPALIVE uint32_t d2common_10a00_engine(uint32_t head, const Cb* c) { return pick(head, 0xE, false, 1, 1, 3, c); }

}
