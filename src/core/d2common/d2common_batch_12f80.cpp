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
// 1.13c D2Common+0x12F80 lives in d2common_12f80.cpp (own file, exact seam signatures).
// 1.13c D2Common+0x69B50: visit every cell of the room (+1 when the edge flag is 0), calling the per-cell hook.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_69b50(uint32_t room, uint32_t grid, int32_t a3, int32_t noEdgeX, int32_t noEdgeY,
  void (*cell)(uint32_t,uint32_t,uint32_t,int32_t,int32_t,int32_t)) {
 uint32_t u = w(w(w(room,0x58),0x1b4),0x478);
 int32_t cx = (noEdgeX == 0) + (int32_t)w(room,0x3c), cy = (noEdgeY == 0) + (int32_t)w(room,0x40);
 for (int32_t y = 0; y < cy; ++y) for (int32_t x = 0; x < cx; ++x)
  cell(u, room, grid ? w(w(grid,0) , (w(w(grid,4), y*4) + x)*4) : 0, w(room,0x34)+x, w(room,0x38)+y, a3);
}
// Allocator that also takes the request size (retail passes it in EDX): 0x34 for the DrlgLogic block,
// count*0x30 for the coord-entry array. A separate typedef so the shared Alloc users stay unchanged.
typedef uint32_t (*AllocSized)(const char*, int, int, uint32_t);
// 1.13c D2Common+0x81CA0: allocate the 0x34-byte DrlgLogic block (+0x64 of level), run the build passes.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_81ca0(uint32_t level, uint32_t ctx, AllocSized alloc,
  void (*p1650)(), void (*p6950)(int,int), void (*p14c0)(), void (*p1bc0)(), void (*p1200)(uint32_t,uint32_t),
  void (*p1a20)(uint32_t), void (*p1ad0)()) {
 const char* F = "..\\Source\\D2Common\\DRLG\\DrlgLogic.cpp";
 uint32_t b = alloc(F, 0xdf, 0, 0x34); memset(p(b), 0, 0x34);
 w(level,100) = b; p1650(); w(b,0) |= 2;
 p6950(w(level,0x3c)+1, w(level,0x40)+1);
 if (!w(ctx,0x1dc)) w(ctx,0x1dc) = 1;
 p14c0(); p1bc0();   // scratch grid (<=0x400 cells) and row-offset table live on retail's stack
 w(b,4) = 1;
 uint32_t a = alloc(F, 0xe9, 0, w(b,4) * 0x30); w(b,0x30) = a; memset(p(a), 0, w(b,4) * 0x30);
 w(ctx,0x1dc) += w(b,4);
 p1200(level, b); p1a20(level); p1ad0();
}
}
