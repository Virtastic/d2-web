// D2Common 1.13c batch. Register-ABI callees and data-table lookups are env seams; the decompile
// loses the implicit EAX/EDX args of 0x98630 so that path is a seam taking the same stack args.
#include <cstdint>
#include <emscripten.h>

extern "C" {
const uint8_t* D2Common_45A40_ItemsRecord(const int32_t* pUnit, const int32_t* pRec);
int32_t d2_fn_d2common_475e0(int32_t in_eax, void* p1, uint32_t p2, int32_t p3, int16_t p4,
                             uint32_t p5, int32_t p6, int32_t p7, uint32_t p8, const uint8_t* dt);

struct D2Common49340Env {
 int32_t (*roll_465b0)(int32_t a, int32_t* pUnit);                    // 0x465B0
 uint32_t (*is_type_24430)(int32_t* pUnit, int32_t type);             // 0x24430
 uint8_t (*is_24c30)(int32_t* pUnit);                                 // 0x24C30
 void (*stat_3a740)(int32_t* pUnit, int32_t stat, uint32_t v);        // 0x3A740
 const uint8_t* (*items_txt)(int32_t id);                             // DATATBLS_GetItemsTxtRecord
 int32_t (*lookup_10213)(int32_t key);                                // Ordinal_10213 -> index (<0 none)
 const uint8_t* types_tbl;                                            // DAT_6fdefb98, 0x1A8 stride
 int32_t (*ratio)(int32_t pct, uint32_t v);                           // DATATBLS_ApplyRatio
 int32_t (*fn_48630)(int32_t* pUnit, int32_t a, int32_t b, int32_t c, int32_t d, int32_t e, int32_t f, int32_t g, uint32_t h); // 0x48630
 void (*fatal)(int32_t line);                                         // 10265+10024 message, then _exit
};

// 1.13c D2Common+0x49340: item param_3 gets min-stat adjustments from its type row, then either
// the 0x48630 path (no ratio gain) or two 0x475E0 notifications. Not an item: both 0x475E0 calls.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_49340(int32_t param_1, void* param_2, int32_t* param_3, int32_t param_4,
                                                  int32_t param_5, int32_t param_6, uint32_t param_7,
                                                  int32_t param_8, int32_t param_9, uint32_t param_10,
                                                  const D2Common49340Env* env, const uint8_t* dt) {
 if (param_4 == 0) return 0;
 if (param_8 == 0) param_8 = env->roll_465b0(*(int32_t*)(uintptr_t)(param_4 + 8), param_3);
 if (!param_3 || *param_3 != 4) {
  d2_fn_d2common_475e0(0, param_2, (uint32_t)(uintptr_t)param_3, param_5, 0x12, 0, param_8, param_9, param_10, dt);
  d2_fn_d2common_475e0(0, param_2, (uint32_t)(uintptr_t)param_3, param_5, 0x11, 0, param_8, param_9, param_10, dt);
  return 1;
 }
 const uint8_t* rec = D2Common_45A40_ItemsRecord(param_3, nullptr);
 if (!rec) return 0;
 // Retail re-validates item-ness and re-fetches the txt row between each step; failure of the
 // re-check exits with line 0x61C, a missing txt row with 0x61F.
 auto typeRow = [&]() -> const uint8_t* {
  if (*param_3 != 4) { env->fatal(0x61c); __builtin_trap(); }
  const uint8_t* t = env->items_txt(param_3[1]);
  if (!t) { env->fatal(0x61f); __builtin_trap(); }
  return t;
 };
 const uint8_t* t = env->items_txt(param_3[1]);
 if (!t) { env->fatal(0x61f); __builtin_trap(); }
 int32_t ix = env->lookup_10213(*(const int32_t*)(t + 0x80));
 if (ix >= 0) {
  const uint8_t* ty = env->types_tbl + (size_t)ix * 0x1a8;
  if (env->is_type_24430(param_3, 0x2d)) {
   if (ty[0xfe]) env->stat_3a740(param_3, 0x15, ty[0xfe]);
   if (ty[0x102]) env->stat_3a740(param_3, 0x17, ty[0x102]);
   if (env->is_24c30(param_3) && ty[0x100]) env->stat_3a740(param_3, 0x9f, ty[0x100]);
  }
 }
 t = typeRow();
 ix = env->lookup_10213(*(const int32_t*)(t + 0x80));
 if (ix >= 0) {
  const uint8_t* ty = env->types_tbl + (size_t)ix * 0x1a8;
  if (env->is_type_24430(param_3, 0x2d)) {
   if (ty[0xff]) env->stat_3a740(param_3, 0x16, ty[0xff]);
   if (ty[0x103]) env->stat_3a740(param_3, 0x18, ty[0x103]);
   if (env->is_24c30(param_3) && ty[0x101]) env->stat_3a740(param_3, 0xa0, ty[0x101]);
  }
 }
 // Four more checked fetch+lookup pairs (results unused in retail except the last for 0x98630).
 for (int i = 0; i < 4; ++i) { t = typeRow(); env->lookup_10213(*(const int32_t*)(t + 0x80)); }
 uint32_t v = rec[0x103];
 if (v <= rec[0xff]) v = rec[0xff];
 if (env->is_type_24430(param_3, 0x2d)) {
  int32_t g = env->ratio(100, v);
  if ((int32_t)(g + v) <= (int32_t)v)
   return env->fn_48630(param_3, param_1, 0, 1, param_6, (int32_t)param_7, 0, 0, 0);
 }
 d2_fn_d2common_475e0(0, param_2, (uint32_t)(uintptr_t)param_3, param_5, 0x12, 0, param_8, param_6, param_7, dt);
 d2_fn_d2common_475e0(0, param_2, (uint32_t)(uintptr_t)param_3, param_5, 0x11, 0, param_5, param_6, param_7, dt);
 return param_5;
}
}
