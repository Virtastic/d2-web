// D2Common 1.13c batch. Retail globals are passed in: dir = DAT_6fddca38 (8-byte entries:
// +0 int16 dx, +4 int16 dy); chk = FUN_6fd9ce90 (D2Common+0x4CE90). Args: a = EAX struct, s = ESI state.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 1.13c D2Common+0x186D0: try direction s[0], then follow s[6] link table up to 3 more times.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_186d0(const int32_t* a, int32_t* s, const int16_t* dir,
  uint32_t (*chk)(int32_t, uint32_t, uint32_t, int32_t, uint32_t)) {
 const int32_t v8 = a[2], v28 = a[10];
 uint32_t v2c = static_cast<uint32_t>(a[11]);
 const int32_t* p = *reinterpret_cast<const int32_t* const*>(a[12] + 0x30); // 32-bit ptr in retail
 if (!p || (*p != 0 && *p != 1)) __builtin_trap(); // retail: error report then _exit(-1)
 const int32_t* link = reinterpret_cast<const int32_t*>(static_cast<intptr_t>(s[6]));
 auto step = [&]() {
  const int32_t d = s[0];
  const uint16_t x = static_cast<uint16_t>(dir[d*4] + static_cast<int16_t>(s[2]));
  const uint16_t y = static_cast<uint16_t>(dir[d*4+2] + reinterpret_cast<const int16_t*>(s)[5]);
  reinterpret_cast<uint16_t*>(s)[6] = x;
  reinterpret_cast<uint16_t*>(s)[7] = y;
  return chk(v8, x, y, v28, v2c);
 };
 s[4] = 0;
 if (!step()) return 1;
 s[0] = link[s[0]];
 if (!step()) return 1;
 s[0] = link[s[0]];
 if (!step()) return 1;
 s[4] = 1;
 s[0] = link[s[0]];
 v2c = step();
 return v2c ? 0 : 1;
}
}
