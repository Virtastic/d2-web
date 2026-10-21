// D2Common 1.13c batch. Resolve a value from p1->+8 via 463F0 (pEax != 0) or 464D0 (pEax == 0), then
// add (or revoke) it on a child stat list. Registers: EAX (pEax), EDI (nStat, packed stat id).
// Callees are seams (retail addresses); fatal = Ordinal_10265(0x29d)/10024 then _exit(-1).
#include <cstdint>
#include <emscripten.h>

extern "C" {
int32_t (*g_d2common_fn_47fa0_resolve463f0)(int32_t a, int32_t* b) = nullptr;  // 0x463F0
int32_t (*g_d2common_fn_47fa0_resolve464d0)(int32_t a, int32_t* b) = nullptr;  // 0x464D0
void (*g_d2common_fn_47fa0_apply46e50)() = nullptr;                            // 0x46E50
void (*g_d2common_fn_47fa0_shift45640)(int32_t* pValue) = nullptr;             // 0x45640
uint32_t (*g_d2common_fn_47fa0_child47020)(uint32_t pOwner, uint32_t id, uint32_t flt) = nullptr; // 0x47020
int32_t (*g_d2common_fn_47fa0_get39140)(uint32_t pList, uint32_t nStat) = nullptr;
void (*g_d2common_fn_47fa0_add3a330)(uint32_t pList, uint32_t nStat, int32_t nDelta) = nullptr;
void (*g_d2common_fn_47fa0_fatal)() = nullptr;

// 1.13c D2Common+0x47FA0
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_47fa0(const int32_t* p1, uint32_t pEax, int32_t* p2, int32_t p3,
                                                   uint32_t pOwner, int32_t bRevoke, uint32_t id, uint32_t flt,
                                                   uint32_t nStat) {
 int32_t v;
 if (pEax == 0) {
  v = g_d2common_fn_47fa0_resolve464d0(p1[2], p2);
  if (p3 == 1) g_d2common_fn_47fa0_apply46e50();
 } else {
  v = g_d2common_fn_47fa0_resolve463f0(p1[2], p2);
  g_d2common_fn_47fa0_apply46e50();
 }
 if (v == 0) return 0;
 g_d2common_fn_47fa0_shift45640(&v);
 const uint32_t child = g_d2common_fn_47fa0_child47020(pOwner, id, flt);
 if (!child) {
  g_d2common_fn_47fa0_fatal();
  __builtin_trap(); // retail: _exit(-1)
 }
 int32_t d = v;
 if (bRevoke) d = -g_d2common_fn_47fa0_get39140(child, nStat);
 g_d2common_fn_47fa0_add3a330(child, nStat, d);
 if (nStat == 0x3a) g_d2common_fn_47fa0_add3a330(child, 0x146, 1);
 return 1;
}
}
