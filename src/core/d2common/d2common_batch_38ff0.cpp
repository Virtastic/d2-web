// D2Common 1.13c batch. Unwired callees and sgptDataTables are weak so the file links.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W int32_t sub_38B00(intptr_t);   // 0x38B00
W int32_t sub_38A80(intptr_t);   // 0x38A80
W uint8_t* sgptDataTables;

// 1.13c D2Common+0x38FF0: stat-table record 0x49 base gate; needs param_1->+0x5C set.
EMSCRIPTEN_KEEPALIVE int32_t sub_38FF0(int32_t param_1) {
 if (param_1 == 0 || *(int32_t*)(sgptDataTables + 0xbd4) <= 0x49) return 0;
 const intptr_t rec = *(int32_t*)(sgptDataTables + 0xbcc) + 0x5c64;
 if (rec == 0 || *(uint32_t*)((intptr_t)(uint32_t)param_1 + 0x5c) == 0) return 0;
 if (sub_38B00(rec) == 0) return 0;
 return sub_38A80(rec);
}
}
