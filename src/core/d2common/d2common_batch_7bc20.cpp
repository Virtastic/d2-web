// D2Common 1.13c batch: room status normalise. Pointers are intptr_t on wasm64.
#include <cstdint>
#include <emscripten.h>

extern "C" {
// 0x7BB70(room in EAX, status in BL): relink the room into the status list; not yet wired.
__attribute__((weak)) void d2_fn_d2common_7bb70(intptr_t pRoom, uint8_t nStatus);

// 1.13c D2Common+0x7BC20: when the room's status byte (+0x44) is >= 4 or its ref count at
// +0xC+status*2 is zero, pick the first status 0..3 with a non-zero count (4 if none), relink via
// 0x7BB70, then store it at +0x44.
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_7bc20(intptr_t pRoom) {
 uint8_t* r = reinterpret_cast<uint8_t*>(pRoom);
 const uint8_t cur = r[0x44];
 if (cur < 4 && reinterpret_cast<uint16_t*>(r + 0xC)[cur] != 0) return;
 uint8_t s = 0;
 do {
  if (reinterpret_cast<uint16_t*>(r + 0xC)[s] != 0) break;
  ++s;
 } while (s < 4);
 if (d2_fn_d2common_7bb70) d2_fn_d2common_7bb70(pRoom, s);
 r[0x44] = s;
}
}
