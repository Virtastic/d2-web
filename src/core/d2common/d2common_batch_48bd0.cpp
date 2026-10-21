// D2Common 1.13c batch. Retail callees are explicit seams (function-pointer table);
// sgptDataTables is passed as a byte pointer.
#include <cstdint>
#include <emscripten.h>

struct D2Common48BD0Env {
 void (*normalize)(int32_t* v);                                      // +0x45640
 const uint8_t* (*itemsTxt)(uint32_t id);                            // DATATBLS_GetItemsTxtRecord
 uint32_t (*unitStat)(const int32_t* unit, int32_t stat);            // +0x74430
 int32_t (*itemRecordId)(const int32_t* unit);                       // +0x730F0
 void (*apply)(int32_t a, const int32_t* u2, int32_t v, int32_t p6, int32_t p7, uint32_t p8); // +0x97EF0
 int32_t* (*findStatList)(const int32_t* u2, int32_t p7, uint32_t p8); // +0x97020
 int32_t (*getStat)(const int32_t* list, int32_t stat);              // +0x89140
 void (*setStat)(int32_t* list, int32_t stat, int32_t v);            // +0x8A330
 void (*fatal)(uint32_t line);                                       // Ordinal_10265/10024 + _exit
 const uint8_t* dataTables;                                          // sgptDataTables
};

extern "C" {
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_48bd0(int32_t p1, const int32_t* p2, const int32_t* p3, const int32_t* p4,
                                                   int32_t p5, int32_t p6, int32_t p7, uint32_t p8, const D2Common48BD0Env* e) { // 1.13c D2Common+0x48BD0
 int32_t local4 = p4[3];
 int32_t v4 = p4[2];
 e->normalize(&v4);
 e->normalize(&local4);
 const int32_t stat = p5;
 if (stat == 0x15) {
  const uint8_t* rec;
  if (p2 && *p2 == 4) rec = e->itemsTxt(static_cast<uint32_t>(p2[1]));
  else rec = e->itemsTxt(p3 ? static_cast<uint32_t>(p3[1]) : 0xffffffffu);
  const uint32_t a = p8;
  if (!e->unitStat(p3, 0x2d) || rec[0xff] != 0 || rec[0x103] == 0) {
   e->apply(p1, p2, v4, p6, p7, a);
   e->apply(p1, p2, local4, p6, p7, a);
  }
  if (!e->unitStat(p3, 0x2d) || rec[0x103] != 0 || rec[0xfe] == 0) {
   e->apply(p1, p2, v4, p6, p7, a);
   e->apply(p1, p2, local4, p6, p7, a);
  }
  if (p3 && *p3 == 4) {
   const int32_t id = e->itemRecordId(p3);
   if (id >= 0 && id < *reinterpret_cast<const int32_t*>(e->dataTables + 0xbfc)) {
    const uint8_t* r = *reinterpret_cast<const uint8_t* const*>(e->dataTables + 0xbf8) + id * 0xe4;
    if (r && r[0x10]) {
     e->apply(p1, p2, v4, p6, p7, a);
     e->apply(p1, p2, local4, p6, p7, a);
     return 1;
    }
   }
  }
 } else {
  int32_t* list = e->findStatList(p2, p7, p8);
  if (!list) { e->fatal(0x485); __builtin_trap(); }
  int32_t x = v4, y = local4;
  if (p6) {
   const int32_t a = e->getStat(list, stat);
   const int32_t b = e->getStat(list, stat + 1);
   x = -a; y = -b;
  }
  e->setStat(list, stat, x);
  e->setStat(list, stat + 1, y);
  if (stat == 0x39) e->setStat(list, 0x146, 1);
 }
 return 1;
}
}
