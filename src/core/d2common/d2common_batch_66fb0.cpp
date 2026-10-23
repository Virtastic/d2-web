// D2Common 1.13c batch. Unwired callees are weak seams so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
__attribute__((weak)) uint32_t d2_sub_4CE90(uint32_t room, uint32_t x, uint32_t y, uint32_t pattern, uint32_t mask);
__attribute__((weak)) uint32_t d2_ord_10265(uint32_t);
__attribute__((weak)) void d2_ord_10024(const void* fmt, uint32_t v);

static void norm_3E110(int32_t* a, int32_t* b) {  // 1.13c D2Common+0x3E110
 if (*a < 0) { while (*a < -0x10000) { *a >>= 1; *b >>= 1; } }
 else { while (*a > 0x10000) { *a >>= 1; *b >>= 1; } }
 if (*b < 0) { while (*b < -0x10000) { *a >>= 1; *b >>= 1; } }
 else { while (*b > 0x10000) { *a >>= 1; *b >>= 1; } }
}

// 1.13c D2Common+0x66FB0  eax=path (+0x1c room, +0x48 pattern, +0x50 mask), this=int[2] step vector.
// Walks cells from fixed-point start `a` (y<<16|x) toward *p; on first collision stores prior cell in *p, returns 0.
EMSCRIPTEN_KEEPALIVE uint32_t d2_sub_66FB0(const uint8_t* eax, void* self, uint32_t a, uint32_t* p) {
 uint32_t room = *reinterpret_cast<const uint32_t*>(eax + 0x1c);
 uint32_t pat = *reinterpret_cast<const uint32_t*>(eax + 0x48);
 uint32_t mask = *reinterpret_cast<const uint32_t*>(eax + 0x50);
 uint32_t dst = *p;
 int32_t px = (int32_t)(a << 16) + 0x8000;
 int32_t py = (int32_t)((a & 0xffff0000u) + 0x8000);
 int32_t adx = (int32_t)((dst & 0xffff) - ((uint32_t)(px >> 16) & 0xffff));
 int32_t ady = (int32_t)((dst >> 16) - ((uint32_t)(py >> 16) & 0xffff));
 int32_t dx = reinterpret_cast<int32_t*>(self)[0], dy = reinterpret_cast<int32_t*>(self)[1];
 if (dx == INT32_MIN || dx == 0) {
  if (dy == INT32_MIN || dy == 0) { d2_ord_10024(nullptr, d2_ord_10265(0x6e)); __builtin_trap(); }
 }
 norm_3E110(&dx, &dy);
 if (adx < 0) adx = -adx;
 if (ady < 0) ady = -ady;
 int16_t tx = (int16_t)dst, ty = (int16_t)(dst >> 16);
 uint32_t cur = a;
 if (ady < adx) {
  while ((int16_t)a != tx) {
   uint32_t prev = cur;
   px += dx; py += dy;
   a = ((uint32_t)(uint16_t)(py >> 16) << 16) | (uint16_t)(px >> 16);
   cur = a;
   if (((int16_t)prev != (int16_t)a || (int16_t)(prev >> 16) != (int16_t)(a >> 16)) &&
       d2_sub_4CE90(room, (px >> 16) & 0xffff, (py >> 16) & 0xffff, pat, mask)) { *p = prev; return 0; }
  }
 } else {
  while ((int16_t)(a >> 16) != ty) {
   uint32_t prev = cur;
   py += dy; px += dx;
   a = ((uint32_t)(uint16_t)(py >> 16) << 16) | (uint16_t)(px >> 16);
   cur = a;
   if (((int16_t)prev != (int16_t)a || (int16_t)(prev >> 16) != (int16_t)(a >> 16)) &&
       d2_sub_4CE90(room, (px >> 16) & 0xffff, (py >> 16) & 0xffff, pat, mask)) { *p = prev; return 0; }
  }
 }
 if (tx == 0) { d2_ord_10024(nullptr, d2_ord_10265(0xab)); __builtin_trap(); }
 if (ty != 0) return 1;
 d2_ord_10024(nullptr, d2_ord_10265(0xac)); __builtin_trap();
}
}
