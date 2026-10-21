// D2Common 1.13c batch. Apply (or revoke) a stat delta on a child stat list of an item. Retail
// registers: EAX (pEax) gates the 0x46E50 item-type application, EDI (nStat) is the packed stat id.
// Callees are seams (retail addresses); fatal = Ordinal_10265(0x2c5)/10024 then _exit(-1).
#include <cstdint>
#include <emscripten.h>

extern "C" {
void (*g_d2common_fn_47ef0_apply46e50)() = nullptr;                          // 0x46E50
void (*g_d2common_fn_47ef0_shift45640)(int32_t* pValue) = nullptr;           // 0x45640 (selector in-register)
uint32_t (*g_d2common_fn_47ef0_child47020)(uint32_t pOwner, uint32_t id, uint32_t flt) = nullptr; // 0x47020
int32_t (*g_d2common_fn_47ef0_get39140)(uint32_t pList, uint32_t nStat) = nullptr;
void (*g_d2common_fn_47ef0_add3a330)(uint32_t pList, uint32_t nStat, int32_t nDelta) = nullptr;
void (*g_d2common_fn_47ef0_fatal)() = nullptr;

// 1.13c D2Common+0x47EF0
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_47ef0(int32_t p1, uint32_t pEax, uint32_t nStat, uint32_t pOwner,
                                                   int32_t nValue, int32_t bRevoke, uint32_t id, uint32_t flt) {
 if (pEax != 0 || p1 == 1) g_d2common_fn_47ef0_apply46e50();
 if (nValue == 0) return 0;
 g_d2common_fn_47ef0_shift45640(&nValue);
 const uint32_t child = g_d2common_fn_47ef0_child47020(pOwner, id, flt);
 if (!child) {
  g_d2common_fn_47ef0_fatal();
  __builtin_trap(); // retail: _exit(-1)
 }
 if (bRevoke) {
  const int32_t cur = g_d2common_fn_47ef0_get39140(child, nStat);
  g_d2common_fn_47ef0_add3a330(child, nStat, -cur);
  return 1;
 }
 g_d2common_fn_47ef0_add3a330(child, nStat, nValue);
 return 1;
}
}
