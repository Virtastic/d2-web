// D2Common 1.13c batch. Retail keeps the search frame on the stack; 32-bit layouts kept.
// Unwired callees are weak so the file links: 0x708F0 (frame walker), 0x3E190 (direction
// index), ordinal 10265/10024 (error log).
#include <cstdint>
#include <emscripten.h>

extern "C" {
#pragma pack(push, 1)
struct D2C70BB0Frame {  // retail stack locals from -0x30d6c up; grid at +0x1c
 int32_t count;
 uint16_t x0, y0;
 int16_t x1, y1;
 uint16_t w, h;
 int32_t stride, offX, offY;
 uint32_t grid[50003];
};
struct D2C70BB0Search {  // 28 bytes copied per attempt (retail -0x36ffc)
 uint16_t dist;
 uint32_t dist32;
 uint16_t marker;
 uint32_t start;
 uint32_t rowTbl;
 uint32_t dir;
 uint32_t z0, z1;
};
#pragma pack(pop)
struct D2C70BB0Env {
 const uint8_t* dirTbl;  // DAT_6fddc320, 12-byte entries
 uint32_t rowTbl;        // DAT_6fddb860 (address value only)
};
__attribute__((weak)) const D2C70BB0Env* d2_env_d2common_70bb0 = nullptr;
__attribute__((weak)) int32_t d2_fn_d2common_3e190(uint32_t a, uint32_t b);
__attribute__((weak)) const int16_t* d2_fn_d2common_708f0(int32_t dist, const uint32_t* p3, D2C70BB0Frame* f, D2C70BB0Search* s);
uint32_t d2_fn_d2common_70650(const uint8_t* owner, const uint8_t* node);

// 1.13c D2Common+0x70BB0: build the (w+6)x(h+6) search frame, retry the walker with growing
// distance (+5) until it finds a node or the limit is hit, then flatten the chain; 0 if >0x4d.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_70bb0(uint32_t owner, const uint16_t* p2, const uint32_t* p3) {
 static D2C70BB0Frame f;
 const int32_t w = *reinterpret_cast<const int32_t*>(p2 + 4), h = *reinterpret_cast<const int32_t*>(p2 + 6);
 if (50000 < w * h) __builtin_trap();  // retail logs then _exit(-1)
 const uint32_t n = static_cast<uint32_t>((w + 6) * (h + 6)) & 0x3fffffff;
 for (uint32_t i = 0; i < n; ++i) f.grid[i] = 0;
 f.w = p2[4]; f.x0 = p2[0]; f.y0 = p2[2]; f.h = p2[6];
 f.x1 = static_cast<int16_t>(f.w + f.x0);
 f.y1 = static_cast<int16_t>(f.h + f.y0);
 f.stride = f.w + 6;
 f.offX = 3 - f.x0;
 f.offY = 3 - f.y0;
 const uint32_t start = p3[0], goal = p3[1];
 int32_t dx = static_cast<int32_t>(goal & 0xffff) - static_cast<int32_t>(start & 0xffff);
 if (dx < 0) dx = -dx;
 int32_t dy = static_cast<int32_t>(goal >> 16) - static_cast<int32_t>(start >> 16);
 if (dy < 0) dy = -dy;
 int32_t lo, hi;
 if (dx >= dy) { lo = dy; hi = dx; } else { lo = dx; hi = dy; }
 const uint16_t d16 = static_cast<uint16_t>(lo + hi * 2);
 D2C70BB0Search s = {};
 s.dist = d16; s.dist32 = d16; s.marker = 0xfffd; s.start = start;
 s.rowTbl = d2_env_d2common_70bb0->rowTbl;
 const int32_t di = d2_fn_d2common_3e190(start, goal);
 s.dir = *reinterpret_cast<const uint32_t*>(d2_env_d2common_70bb0->dirTbl + di * 12) & 7;
 int32_t cur = static_cast<int16_t>(d16), limit;
 const uint8_t b = static_cast<uint8_t>(p3[7]);
 if (p3[8] == 0) {
  limit = static_cast<int16_t>(b > cur ? b : cur);
 } else {
  limit = cur;
  if (p3[8] == 0x10) {
   cur = static_cast<int16_t>(static_cast<int16_t>(d16) / 2 + static_cast<int16_t>(s.dist32));
   limit = static_cast<int16_t>(b > cur ? b : cur);
  }
 }
 const int16_t* r;
 do {
  f.count = 1;
  D2C70BB0Search t = s;
  r = d2_fn_d2common_708f0(cur, p3, &f, &t);
  cur += 5;
 } while (((r || f.count != 900) && (f.count = 1, !r)) && cur < limit);
 uint32_t out = 0;
 if (r) {
  out = d2_fn_d2common_70650(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(owner)), reinterpret_cast<const uint8_t*>(r));
  if (out > 0x4d) out = 0;
 }
 return out;
}
}
