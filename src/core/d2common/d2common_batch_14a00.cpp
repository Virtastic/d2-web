// D2Common 1.13c batch. Retail globals are env-struct pointers: tbl = DAT_6fddcbec (16-byte
// entries: +0 kind, +4 link), grid = DAT_6fde54b8.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x14A00: check entry p2 against the other entries' boxes, then kind rule.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_14a00(const int32_t* a, int32_t p2, const int32_t* tbl, const int32_t* grid) {
 const int32_t link = tbl[p2*4+1];
 const int32_t* q = a + 3;
 for (int32_t i = 0; i < p2; ++i, q += 4) {
  if (i == link) continue;
  int32_t dx = a[p2*4+2], x = q[-1];
  dx = dx < x ? (x - a[p2*4+4]) - dx : (dx - q[1]) - x;
  int32_t dy = a[p2*4+3], y = *q;
  dy = dy < y ? (y - a[p2*4+5]) - dy : (dy - q[2]) - y;
  if (dx < 0 && dy < 0) return 0;
 }
 const int32_t kind = tbl[p2*4];
 if (kind == 1) return static_cast<uint32_t>(grid[a[0x3F+p2] + (a[0x5D+p2] + (a[0x3F+link] + a[0x5D+link]*4)*2)*4]);
 if (kind == 0x11)
  for (int32_t i = 0; i < 15; ++i)
   if (i != p2 && tbl[i*4+1] == link && a[0x3F+p2] == a[0x3F+i]) return 0;
 return 1;
}
}
