// D2Common 1.13c batch. Units are addressed as uint32 words (+0x00 type, +0x04 class, +0x14 data ptr,
// +0x60 inventory). Item data +0x44 = item type class, +0x18 = flags.
#include <cstdint>
#include <emscripten.h>

extern "C" {
#define W __attribute__((weak))
W intptr_t sub_1E1B0(const char* env);                 // 0x1E1B0
W uint32_t sub_22E70(uint32_t idx);                    // 0x22E70
W int32_t sub_30530(uint32_t* u);                      // 0x30530
W intptr_t sub_20320(uint32_t inv);                    // 0x20320
W intptr_t DATATBLS_GetItemsTxtRecord(int32_t);
W uint32_t Ordinal_10265(uint32_t);
W void Ordinal_10024(const void*, uint32_t);
W void D2Common_1B0D_Exit(int32_t);
W char DAT_6fded378[1];
W char DAT_6fdda728[1];

static inline const uint32_t* U(uint32_t v) { return (const uint32_t*)(intptr_t)v; }
// Equipped-slot unit at off in the inventory record, nulled if the item is flagged 0x100/0x4000.
static const uint32_t* slot(const uint32_t* inv, uint32_t off) {
 if (!inv || inv[0] != 0x1020304) return nullptr;
 const intptr_t e = sub_1E1B0(DAT_6fded378);
 if (!e) return nullptr;
 const uint32_t* p = U(*(const uint32_t*)(intptr_t)(*(const uint32_t*)(e + 0xc) + off));
 if (!p) return nullptr;
 if (p[0] == 4 && p[5]) {
  const uint32_t f = *(const uint32_t*)(intptr_t)(p[5] + 0x18);
  if (f & 0x100 || f & 0x4000) return nullptr;
 }
 return p;
}

// 1.13c D2Common+0x31260: item-unit type-class byte, or 6 when a dual-wield equipped pair makes it so.
EMSCRIPTEN_KEEPALIVE uint8_t sub_31260(uint32_t* pl, const uint32_t* item) {
 uint8_t cls = 0;
 if (item && item[0] == 4 && item[5]) cls = *(const uint8_t*)(intptr_t)(item[5] + 0x44);
 if (cls == 5 || cls == 4) {
  const uint32_t* inv = U(pl[0x18]);
  const uint32_t* a = slot(inv, 0x10);
  const uint32_t* b = slot(inv, 0x14);
  if (a && b && (a[0] != 4 || !sub_22E70(a[1])) && (b[0] != 4 || !sub_22E70(b[1])) &&
      sub_30530(pl) && (const uint32_t*)sub_20320(pl[0x18]) != item)
   return 6;
 }
 uint32_t code;
 if (!item) code = 0x635;
 else if (item[0] == 4) {
  const intptr_t r = DATATBLS_GetItemsTxtRecord((int32_t)item[1]);
  if (r) return *(const uint8_t*)(r + 0x115);
  code = 0x639;
 } else code = 0x636;
 Ordinal_10024(DAT_6fdda728, Ordinal_10265(code));
 D2Common_1B0D_Exit(-1);
 __builtin_trap();
}
}
