// D2Common 1.13c batch: CRT __cftoe wrapper (format double into buffer, 'e' style).
#include <cstdint>
#include <emscripten.h>

extern "C" {
// Unwired callees (weak): FUN_6fdd7a91 (_fltout2-like), __fptostr, __cftoe2.
__attribute__((weak)) intptr_t d2_fn_d2common_87a91(uint32_t flt, uint32_t p2, int32_t* strflt, uint32_t* buf6);
__attribute__((weak)) intptr_t d2_fn_d2common_87960(char* buf, uint32_t size, int32_t ndig, uint32_t strflt);
__attribute__((weak)) void d2_fn_d2common_87226(intptr_t flt, intptr_t buf, int32_t nDigits, int32_t bCaps, char bG);

// 1.13c D2Common+0x872D4
EMSCRIPTEN_KEEPALIVE int32_t d2common_872d4(uint32_t* param_1, int32_t param_2, int32_t param_3, int32_t param_4) {
 int32_t local_30[4];
 uint32_t local_20[6];
 const uint32_t ptflt = param_1[0];
 d2_fn_d2common_87a91(ptflt, param_1[1], local_30, local_20);
 d2_fn_d2common_87960(
  reinterpret_cast<char*>(static_cast<uintptr_t>((0 < param_3 ? 1u : 0u) + (local_30[0] == 0x2d ? 1u : 0u) + static_cast<uint32_t>(param_2))),
  static_cast<uint32_t>(param_3 + 1), static_cast<uint32_t>(reinterpret_cast<intptr_t>(local_30)), ptflt);
 d2_fn_d2common_87226(reinterpret_cast<intptr_t>(local_30), param_2, param_3, param_4, 0);
 return param_2;
}
}
