// D2Common 1.13c implementation batch (DRLG anim / logic / room neighbours). Register-carried
// inputs are explicit parameters; unresolved retail callees are callback seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
typedef uint32_t (*Alloc)(const char*, int, int);
}

extern "C" {
// 1.13c D2Common+0x12F80: build DrlgAnim records for each 0x30-byte cell entry (array base in EAX)
// whose type has bit 0x100; the first picked tile fills the entry, the rest become linked objects.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_12f80(uint32_t arr, uint32_t drlg, int32_t dflt, int32_t count, uint32_t tbl,
  uint32_t (*typeFlags)(uint32_t), uint32_t (*pick)(uint32_t,uint32_t,uint32_t,uint32_t,int32_t*,int),
  Alloc alloc, int (*same)(int32_t,uint32_t), uint32_t (*rank)(int32_t),
  uint32_t (*mk0)(int32_t,int32_t,uint32_t,int32_t), uint32_t (*mkD)(uint32_t,int32_t,uint32_t,int32_t,int32_t),
  uint32_t (*mkN)(uint32_t,uint32_t,int32_t,int32_t,uint32_t,int32_t,int32_t), void (*fatal)(int)) {
 if (!arr) return;
 if (!dflt) dflt = 0x50;
 const char* F = "..\\Source\\D2Common\\DRLG\\DrlgAnim.cpp";
 int32_t t[40];
 uint32_t e = arr + 0x18;
 for (int i = 0; i < count; ++i, e += 0x30) {
  if (!w(e,0) || !(typeFlags(w(e,0)) & 0x100)) continue;
  uint32_t g = tbl - 0x14 + ((w(e,-4) >> 14) & 7) * 0x14;
  uint32_t v = w(w(g,0) + (w(w(g,4) + w(e,-12)*4, 0) + w(e,-16))*4, 0);
  uint32_t bt = v ? (v >> 20) & 0x3f : 0, ix = v ? v >> 8 : 0;
  uint32_t n = pick(drlg + 0x68, w(e,4), bt, ix & 0xff, t, 0x28);
  uint32_t r = alloc(F, 0x9b, 0);
  memset(p(r), 0, 20); w(r,4) = n;
  w(r,0) = alloc(F, 0xa1, 0); memset(p(w(r,0)), 0, n * 4);
  w(r,12) = dflt; uint32_t lvl = w(drlg,0x54);
  w(r,16) = w(lvl,4); w(lvl,4) = r;
  uint32_t k = 0;
  while (k < n && same(t[k], n)) ++k;
  if (k >= n) { fatal(0xb6); return; }
  w(e,0) = t[k]; w(w(r,0),0) = e - 24;
  uint32_t x = w(e,-12) + w(drlg,0x38), y = w(e,-16) + w(drlg,0x34);
  for (uint32_t s = 1; s < n; ++s) {
   uint32_t j = 0;
   while (j < n && rank(t[j]) != s) ++j;
   if (j >= n) { fatal(0xcb); return; }
   uint32_t o, ty = w(e,4);
   if (!ty) { o = w(lvl,0x1c) + w(lvl,0xc)*0x30; w(o,32) = 0; w(lvl,0xc)++; mk0(x,y,v,t[j]); }
   else if (ty == 0xd) o = mkD(drlg,y,x,0,t[j]);
   else o = mkN(drlg,0,y,x,v,t[j],ty);
   w(w(r,0), s*4) = o; w(o,20) |= 8;
  }
 }
}
// 1.13c D2Common+0x69B50: visit every cell of the room (+1 when the edge flag is 0), calling the per-cell hook.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69b50(uint32_t room, uint32_t grid, int32_t a3, int32_t noEdgeX, int32_t noEdgeY,
  void (*cell)(uint32_t,uint32_t,uint32_t,int32_t,int32_t,int32_t)) {
 uint32_t u = w(w(w(room,0x58),0x1b4),0x478);
 int32_t cx = (noEdgeX == 0) + (int32_t)w(room,0x3c), cy = (noEdgeY == 0) + (int32_t)w(room,0x40);
 for (int32_t y = 0; y < cy; ++y) for (int32_t x = 0; x < cx; ++x)
  cell(u, room, grid ? w(w(grid,0) , (w(w(grid,4), y*4) + x)*4) : 0, w(room,0x34)+x, w(room,0x38)+y, a3);
}
// 1.13c D2Common+0x81CA0: allocate the 0x34-byte DrlgLogic block (+0x64 of level), run the build passes.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_81ca0(uint32_t level, uint32_t ctx, Alloc alloc,
  void (*p1650)(), void (*p6950)(int,int), void (*p14c0)(), void (*p1bc0)(), void (*p1200)(uint32_t,uint32_t),
  void (*p1a20)(uint32_t), void (*p1ad0)()) {
 const char* F = "..\\Source\\D2Common\\DRLG\\DrlgLogic.cpp";
 uint32_t b = alloc(F, 0xdf, 0); memset(p(b), 0, 52);
 w(level,100) = b; p1650(); w(b,0) |= 2;
 p6950(w(level,0x3c)+1, w(level,0x40)+1);
 if (!w(ctx,0x1dc)) w(ctx,0x1dc) = 1;
 p14c0(); p1bc0();   // scratch grid (<=0x400 cells) and row-offset table live on retail's stack
 w(b,4) = 1;
 uint32_t a = alloc(F, 0xe9, 0); w(b,0x30) = a; memset(p(a), 0, w(b,4) * 12);
 w(ctx,0x1dc) += w(b,4);
 p1200(level, b); p1a20(level); p1ad0();
}
// 1.13c D2Common+0x444F0: flag 0x800000 on the room when a neighbour test hits (ECX = start, EDX = room).
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_444f0(uint32_t room, int32_t start, int (*test)(uint32_t,int32_t)) {
 if (test(room,start) == 0 && 0 < (int32_t)w(room,0x2c)) {
  while (test(room,start) == 0) if ((int32_t)w(room,0x2c) <= start + 1) return;
  w(room,0x28) |= 0x800000;
 }
}
// 1.13c D2Common+0x44DF0: collect rooms of the level within 5 tiles on both axes, sort, store list at +8.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_44df0(uint32_t room, void (*sortList)(uint32_t*), Alloc alloc) {
 uint32_t tmp[32]; int32_t n = 0;
 for (uint32_t o = w(w(room,0x58),0x10); o; o = w(o,0x24)) {
  int32_t dx = (int32_t)w(room,0x34) < (int32_t)w(o,0x34) ? (int32_t)w(o,0x34)-(int32_t)w(room,0x3c)-(int32_t)w(room,0x34)
                                                           : (int32_t)w(room,0x34)-(int32_t)w(o,0x3c)-(int32_t)w(o,0x34);
  int32_t dy = (int32_t)w(room,0x38) < (int32_t)w(o,0x38) ? (int32_t)w(o,0x38)-(int32_t)w(room,0x40)-(int32_t)w(room,0x38)
                                                           : (int32_t)w(room,0x38)-(int32_t)w(o,0x40)-(int32_t)w(o,0x38);
  if (dx < 6 && dy < 6) tmp[n++] = o;
 }
 w(room,0x2c) = n;
 sortList(tmp);
 w(room,8) = alloc("..\\Source\\D2Common\\DRLG\\DrlgRoom.cpp", 0x2cb, 0);
 for (int32_t i = 0; i < n; ++i) w(w(room,8), i*4) = tmp[i];
}
}
