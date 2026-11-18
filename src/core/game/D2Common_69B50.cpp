// 1.13c D2Common+0x69B50: sweep the room's cell rectangle, handing each pair of grid cells to the placement dispatcher (0x69820).
// Registers: EDI = room. Stack: grid A, grid B (or 0), a3, noEdgeX, noEdgeY (ret 0x14). Each grid is {+0 cell dwords, +4 row-offset dwords}.
// The dispatcher takes EAX = cell A and stack (u, room, cell B, x, y, a3); it is an injectable callee.
#include <cstdint>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
}

extern "C" {
// 1.13c D2Common+0x69B50
EMSCRIPTEN_KEEPALIVE void d2_port_d2common_69b50(uint32_t room, uint32_t gridA, uint32_t gridB, int32_t a3,
    int32_t noEdgeX, int32_t noEdgeY,
    void (*cell)(uint32_t eaxCellA, uint32_t u, uint32_t room, uint32_t cellB, int32_t x, int32_t y, int32_t a3)) {
 const uint32_t u = w(w(w(room, 0x58), 0x1b4), 0x478);
 const int32_t x0 = (int32_t)w(room, 0x34), y0 = (int32_t)w(room, 0x38);
 const int32_t cx = (noEdgeX == 0) + (int32_t)w(room, 0x3c), cy = (noEdgeY == 0) + (int32_t)w(room, 0x40);
 for (int32_t y = 0; y < cy; ++y)
  for (int32_t x = 0; x < cx; ++x) {
   const uint32_t a = w(w(gridA, 0), (int32_t)(w(w(gridA, 4), y * 4) + x) * 4);
   const uint32_t b = gridB ? w(w(gridB, 0), (int32_t)(w(w(gridB, 4), y * 4) + x) * 4) : 0;
   cell(a, u, room, b, x0 + x, y0 + y, a3);
  }
}
}
