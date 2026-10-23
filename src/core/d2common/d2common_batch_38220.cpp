// D2Common 1.13c batch. Sorted-table lower-bound search; 32-bit retail layout.
#include <cstdint>
#include <emscripten.h>
extern "C" {
// 1.13c D2Common+0x38220: table in EAX (+0 ptr to 8-byte entries keyed by int32 at +0, +4 int16 count),
// key in EBX. Returns the index of an exact match, else the first index whose key is greater, else -1.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_38220(intptr_t pTable, int32_t key) {
 const int32_t n = *reinterpret_cast<int16_t*>(pTable + 4);
 const intptr_t base = (intptr_t)*reinterpret_cast<uint32_t*>(pTable);
 int32_t lo = 0, hi = n;
 while (lo < hi) {
  const int32_t mid = (hi - lo) / 2 + lo;
  const int32_t v = *reinterpret_cast<int32_t*>(base + mid * 8);
  if (v < key) lo = mid + 1;
  else if (v > key) hi = mid;
  else return mid;
 }
 return lo < n ? lo : -1;
}
}
