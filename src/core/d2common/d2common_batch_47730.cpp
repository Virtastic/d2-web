// D2Common 1.13c batch: 0x47730. Item unit (kind 4) with item data whose item flags carry
// 0x00400000 gets its base stats seeded (0x46A40) after the 0x45AA0 call with (7, 0).
#include <cstdint>
#include <emscripten.h>

struct D2Common45AA0Env;
extern "C" {
__attribute__((weak)) void d2_fn_d2common_45aa0(int32_t, int32_t, const int32_t*, const int32_t*, const D2Common45AA0Env*);
__attribute__((weak)) void d2_fn_d2common_46a40(uint32_t);

// 1.13c D2Common+0x47730
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_47730(const int32_t* pUnit) {
 if (!pUnit) return;
 d2_fn_d2common_45aa0(7, 0, pUnit, nullptr, nullptr);
 if (pUnit[0] == 4 && pUnit[5] != 0 &&
     (*reinterpret_cast<const uint32_t*>(static_cast<intptr_t>(static_cast<uint32_t>(pUnit[5])) + 0x18) & 0x400000) != 0)
  d2_fn_d2common_46a40(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pUnit)));
}
}
