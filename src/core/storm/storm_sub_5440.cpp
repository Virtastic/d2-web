// 1.13c Storm+0x5440: resize a table of 128-byte entries (EAX=n, EBX=table {count, oldCount, data}).
// Retail tries SMemReAlloc(data, n<<7, flag 0x10 = no-move); on failure it allocs fresh, copies
// min(oldCount, n) entries, frees the old block. Host seam: calloc/free (as storm_batch_1520.cpp).
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
struct StormTbl128 { uint32_t count, oldCount; uint8_t* data; };
extern "C" EMSCRIPTEN_KEEPALIVE void Storm_Sub5440(StormTbl128* t, uint32_t n) {
  uint8_t* old = t->data;
  t->count = n;
  uint8_t* fresh = (uint8_t*)std::calloc(1, n ? (size_t)n << 7 : 1);   // no-move realloc never succeeds here
  t->data = fresh;
  if (old) {
    uint32_t c = t->oldCount < n ? t->oldCount : n;
    if (c) std::memcpy(fresh, old, (size_t)c << 7);
    std::free(old);
  }
}
