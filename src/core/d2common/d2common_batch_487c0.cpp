// D2Common 1.13c batch. Retail fastcall args (ecx, edx, stack...) are passed in order; the implicit
// EAX values the callees read are not recoverable from this body, so the 0x465B0 and 0x24430
// callees are env seams (register-ABI ports) and EAX for 0x475E0 is passed through as param_4.
#include <cstdint>
#include <emscripten.h>

#include "D2Items.h"

extern "C" {
const uint8_t* D2Common_45A40_ItemsRecord(const int32_t* pUnit, const int32_t* pRec);
int32_t d2_fn_d2common_475e0(int32_t in_eax, void* p1, uint32_t p2, int32_t p3, int16_t p4,
                             uint32_t p5, int32_t p6, int32_t p7, uint32_t p8, const uint8_t* dt);
uint8_t D2Common_ItemIsThrowable_24C30(uint32_t pItem);

struct D2Common487C0Env {
 int32_t (*roll_465b0)(int32_t param_1, int32_t* pUnit);    // 0x465B0
};

// 1.13c D2Common+0x487C0: apply a stat-list adjustment (stats 0x15, 0x17, 0x9f) for item param_3,
// each clamped so the item's min byte (+0xFE / +0x102 / +0x100) plus the delta stays >= 1.
// Retail's _exit paths lie inside the callees and are not modelled here.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_487c0(void* param_2, int32_t* param_3, int32_t param_4, int32_t param_5,
                                                  int32_t param_8, int32_t param_9, uint32_t param_10,
                                                  const D2Common487C0Env* env, const uint8_t* dt) {
 if (param_4 == 0) return 0;
 if (!param_3 || *param_3 != 4) return 0;
 const uint8_t* row = D2Common_45A40_ItemsRecord(param_3, nullptr);
 if (!row) return 0;
 if (param_8 == 0) param_8 = env->roll_465b0(*(int32_t*)(uintptr_t)(param_4 + 8), param_3);
 auto apply = [&](int off, int stat) {
  const uint8_t b = row[off];
  int32_t v = param_8;
  if (b != 0 && (int32_t)(b + param_8) < 1) v = 1 - (int32_t)b;
  if (v != 0) d2_fn_d2common_475e0(param_4, param_2, (uint32_t)(uintptr_t)param_3, param_5, (int16_t)stat, 0, v, param_9, param_10, dt);
 };
 const bool ty = ITEMS_CheckItemTypeId(reinterpret_cast<const D2UnitStrc*>(param_3), 0x2d) != 0;
 if (!ty || row[0xfe] != 0 || row[0x102] == 0) apply(0xfe, 0x15);
 if (!ty || row[0x102] != 0 || row[0xfe] == 0) apply(0x102, 0x17);
 if (ty && D2Common_ItemIsThrowable_24C30((uint32_t)(uintptr_t)param_3) == 0) return param_8;
 apply(0x100, 0x9f);
 return param_8;
}
}
