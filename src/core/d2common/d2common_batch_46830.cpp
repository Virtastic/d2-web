// 1.13c D2Common implementation batch: +0x46830.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"

extern "C" {
// Env seams for still-unnamed callees (32-bit retail address model).
extern uint32_t (*g_d2common_fn_24610)(void*);                 // item socket/affix count (low byte used)
extern void (*g_d2common_fn_3A740)(uint32_t, uint32_t, int32_t); // stat setter (unit, stat, value)
int32_t (*g_d2common_fn_465B0)(int32_t, void*) = nullptr;       // count rule from (arg+8, item)

// 1.13c D2Common+0x46830: roll/apply the number of sockets on an item (pItem->dwUnitType 4).
// p2 points at a record (+4 default, +8 rule arg); nReq is the caller's requested cap (stack arg).
// Returns the sockets set, else 0. ItemsTxt +0x10F/+0x110 are the socket-grid dims whose product caps it at 6.
EMSCRIPTEN_KEEPALIVE uint32_t d2_fn_d2common_46830(int32_t* pItem, uint32_t p2, int32_t nReq) {
 if (p2 == 0) return 0;
 if (pItem == nullptr || pItem[0] != 4) return 0;
 uint32_t a = 0, b = 0;
 const uint8_t* txt = reinterpret_cast<const uint8_t*>(DATATBLS_GetItemsTxtRecord(pItem[1]));
 if (txt) { a = txt[0x10F]; b = txt[0x110]; }
 uint32_t cap = a * b;
 if (cap > 6) cap = 6;
 else if (cap == 0) return 0;
 const uint32_t have = g_d2common_fn_24610(pItem) & 0xff;
 if (have <= cap) cap = have;
 const int32_t* rec = reinterpret_cast<const int32_t*>(static_cast<uintptr_t>(p2));
 if (nReq < 1) {
  nReq = g_d2common_fn_465B0(rec[2], pItem);
  if (nReq < 1) nReq = rec[1];
 }
 if (nReq < 2) nReq = 1;
 if (nReq < static_cast<int32_t>(cap)) cap = static_cast<uint32_t>(nReq);
 if (static_cast<int32_t>(cap) <= 0) return 0;
 if (pItem[0] == 4 && pItem[5] != 0)
  *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(static_cast<uint32_t>(pItem[5])) + 0x18) |= 0x800;
 g_d2common_fn_3A740(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(pItem)), 0xC2, static_cast<int32_t>(cap));
 return cap;
}
}
