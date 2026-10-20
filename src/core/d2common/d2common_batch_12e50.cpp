// D2Common 1.13c implementation batch (0x12E50, 0x86000). Register inputs (EAX/ESI/EDI) are
// explicit parameters; unresolved callees (ordinals 10059/10051, 0x42A70) are callback seams.
#include <cstdint>
#include <emscripten.h>

namespace {
uint8_t* p(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
uint32_t& w(uint32_t a, int o) { return *reinterpret_cast<uint32_t*>(p(a) + o); }
}

extern "C" {
// 1.13c D2Common+0x12E50: walk the (h+extra) x (w+extra) cell grid; for cells with flags
// 0x8000003 look up a 0x28-byte record via ord 10059, and if its ord-10051 flags have 0x100,
// add (count-1) to the totals at +0x28 / +0x18 / +0x20 of ctx+0x54 and set 0x8000000 in ctx+0x28.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_12e50(
    int32_t eax, int32_t edx, uint32_t ctx, uint32_t flagGrid, uint32_t idGrid, uint32_t defId,
    int32_t (*lookup)(uint32_t, uint32_t, uint8_t, uint32_t, uint32_t*, int32_t),
    uint32_t (*recFlags)(uint32_t)) {
 const int32_t cols = (eax == 0) + (int32_t)w(ctx,0x3C);
 const uint32_t sums = w(ctx,0x54);
 const int32_t rows = (int32_t)w(ctx,0x40) + (edx == 0);
 uint32_t rec[40];
 for (int32_t r = 0; r < rows; ++r)
  for (int32_t c = 0; c < cols; ++c) {
   const uint32_t f = w(w(flagGrid,0)+ (w(w(flagGrid,4)+r*4,0)+c)*4, 0);
   if (!(f & 0x8000003)) continue;
   uint32_t id = defId;
   if (idGrid) id = w(w(idGrid,0)+ (w(w(idGrid,4)+r*4,0)+c)*4, 0);
   const uint8_t b = f ? (uint8_t)((f >> 0x14) & 0x3f) : 0;
   const uint32_t u = f ? f >> 8 : 0;
   int32_t n = lookup(ctx+0x68, id, b, u & 0xff, rec, 0x28);
   if (n && (recFlags(rec[0]) & 0x100)) {
    --n;
    if (f & 2) w(sums,0x20) += n; else if (f & 1) w(sums,0x18) += n; else w(sums,0x28) += n;
    w(ctx,0x28) |= 0x8000000;
   }
  }
}
// 1.13c D2Common+0x86000: ESI = unit; its +0x14 data flags 0x400000 / 0x800000 each fire 0x42A70.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_86000(uint32_t unit, void (*f42a70)(uint32_t,int32_t,int32_t,int32_t,int32_t,int32_t)) {
 const uint32_t fl = w(w(unit,0x14),0);
 if (fl & 0x400000) f42a70(unit,0,1,0x31e,-1,0);
 if (fl & 0x800000) f42a70(unit,0,4,0x31e,-1,0);
}
}
