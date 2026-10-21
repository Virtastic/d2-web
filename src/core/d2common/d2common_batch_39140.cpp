// D2Common 1.13c batch. Unwired callee and sgptDataTables are weak so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W int32_t sub_38B00(intptr_t);   // 0x38B00
W uint8_t* sgptDataTables;

// 1.13c D2Common+0x39140 (ordinal 10680): stat-table record lookup by index, 0x144-byte rows.
EMSCRIPTEN_KEEPALIVE int32_t sub_39140(int32_t param_1, int32_t param_2) {
 if (param_1 == 0) return 0;
 if (param_2 > -1 && param_2 < *(int32_t*)(sgptDataTables + 0xbd4)) {
  const intptr_t rec = param_2 * 0x144 + *(int32_t*)(sgptDataTables + 0xbcc);
  if (rec != 0) return sub_38B00(rec);
 }
 return 0;
}
}
