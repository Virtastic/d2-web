// D2Common 1.13c batch. Retail passes the path-state struct in EBX; tables and callees are
// env pointers: dirTab = DAT_6fddc320 (12-byte rows), stepTab = DAT_6fddc2e0 (int16 dx,dy pairs),
// f3e190 = FUN_6fd8e190, f671b0 = FUN_6fdb71b0.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x674C0: walk from s[0] toward s[1] up to s[6] steps, appending a waypoint
// to the path (s[0xc]) at each direction change.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_674c0(uint32_t* s, const int32_t* dirTab, const int16_t* stepTab,
  int32_t (*f3e190)(uint32_t, uint32_t), int32_t (*f671b0)(uint32_t, int32_t*, int32_t*)) {
 const int32_t cnt = static_cast<int32_t>(s[6]);
 uint32_t* const p = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(s[0xc]));
 int32_t n = static_cast<int32_t>(p[0x28 / 4]);
 const uint32_t tgt = s[1];
 uint32_t cur = s[0];
 bool changed = false;
 const int32_t base = static_cast<int32_t>(p[0x98 / 4]);
 int32_t last = 0xff, i = 0;
 int32_t c[3], dir = 0;
 if (cnt > 0) {
  uint32_t* out = &p[0x9c / 4 + n];
  for (;;) {
   const uint32_t prev = cur;
   if (static_cast<int16_t>(cur) == static_cast<int16_t>(tgt) && static_cast<int16_t>(cur >> 16) == static_cast<int16_t>(tgt >> 16)) break;
   const int32_t r = f3e190(cur, tgt) * 3;
   c[0] = (dirTab[r] + base) & 7;
   c[1] = (dirTab[r + 1] + base) & 7;
   c[2] = (dirTab[r + 2] + base) & 7;
   if (f671b0(cur, c, &dir) == 0 || ((dir - 4) & 7) == last) goto fin;
   cur = (static_cast<uint32_t>(static_cast<uint16_t>(static_cast<int16_t>(cur >> 16) + stepTab[dir * 2 + 1])) << 16) |
         static_cast<uint16_t>(static_cast<int16_t>(cur) + stepTab[dir * 2]);
   changed = dir != last;
   if (changed) { *out++ = prev; ++n; }
   ++i;
   last = dir;
   if (cnt <= i) break;
  }
  if (!changed) {
fin:
   if (i != 0) { p[0x9c / 4 + n] = cur; ++n; }
  }
 }
 p[0x28 / 4] = static_cast<uint32_t>(n);
}
}
