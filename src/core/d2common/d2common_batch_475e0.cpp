// D2Common 1.13c batch. Retail global sgptDataTables is passed in as a byte pointer; in_EAX is
// retail's implicit register argument. Callees are the sibling RVA ports.
#include <cstdint>
#include <emscripten.h>
#include "D2StatList.h"

void D2Common_3A330_AddStatLayer(D2StatListStrc* pStatList, int32_t nStatId, int32_t nDelta, uint16_t nLayer); // 0x3A330 (d2common_batch_31d20.cpp)

extern "C" {
uint32_t D2Common_47020_GetOrAddChild(uint32_t pThis, uint32_t pEax, uint32_t id, uint32_t flt); // ECX=this, EAX=in_EAX (retail __thiscall + implicit EAX)
int32_t __attribute__((weak)) d2_fn_d2common_382b0(uint32_t list, uint32_t id); // retail: list in EAX, key in EDI; guessed list=u[9] holder, unwired
void* __attribute__((weak)) d2_fn_d2common_38640();
void __attribute__((weak)) d2_fn_d2common_39f70(int32_t a, int32_t b);
void __attribute__((weak)) d2_fn_d2common_38550();

// 1.13c D2Common+0x475E0
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_475e0(int32_t in_eax, void* p1, uint32_t p2, int32_t p3, int16_t p4,
                                                  uint32_t p5, int32_t p6, int32_t p7, uint32_t p8, const uint8_t* dt) {
 (void)p2;
 if (in_eax == 0 || p6 == 0) return 0;
 const int32_t idx = p4;
 const int32_t cnt = *(const int32_t*)(dt + 0xbd4);
 if (idx < 0 || idx >= cnt) return 0;
 const uint8_t* row = *(const uint8_t* const*)(dt + 0xbcc) + idx * 0x144;
 if (!row) return 0;
 int32_t* u = (int32_t*)(uintptr_t)D2Common_47020_GetOrAddChild((uint32_t)(uintptr_t)p1, (uint32_t)in_eax, (uint32_t)p7, p8);
 if (!u) return 0;
 const uint32_t sh = row[0x18] & 0x1f;
 const int32_t v = (int32_t)((uint32_t)p6 << sh);
 if (p3 == 0) {
  D2Common_3A330_AddStatLayer((D2StatListStrc*)u, idx, v, (uint16_t)p5); // p5 = the layer word 0x3A330 takes last
  if (p4 == 0x3a) D2Common_3A330_AddStatLayer((D2StatListStrc*)u, 0x146, 1, 0);
 } else {
  STATLIST_SetStat((D2StatListStrc*)u, idx, v, 0); // retail FUN_6fd8a280 passes the bare row index, layer 0
  if (p4 == 0x3a && STATLIST_GetStatValue((D2StatListStrc*)u, 0x146, 0) == 0) {
   int32_t i = d2_fn_d2common_382b0((uint32_t)(intptr_t)u, 0x146);
   int32_t* e = i < 0 ? nullptr : (int32_t*)((uintptr_t)u[9] + (uint32_t)i * 8);
   if (!e) e = (int32_t*)d2_fn_d2common_38640();
   const int32_t old = e[1];
   if (1 - old != 0) {
    e[1] = 1;
    d2_fn_d2common_39f70(1 - old, 0);
    if (u[4] < 0 && u[2] == 0) { d2_fn_d2common_38550(); return p6; }
   }
  }
 }
 return p6;
}
}
