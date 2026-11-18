// 1.13c Storm+0x5440: resize a table of 128-byte entries (EAX=n, EBX=table {count, oldCount, data}; the table is three dwords).
// Retail: data = SMemReAlloc(old, n<<7, "AU_PLAYERNAME", -2, 0x10) (0x1C760, no-move); when that fails it allocs a fresh block
// (0x1C580, same tag, flags 0), copies min(oldCount, n) entries from the old block and frees the old block (0x1B960).
// Seams are the Storm allocator (src/core/storm/storm_alloc_1b960.cpp, storm_batch_k04sto.cpp), so an FN probe logs them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

extern "C" void* Storm_SMemReAlloc(void* p, uint32_t size, const char* src, int32_t line, uint32_t f);   // 0x1C760
extern "C" void* Storm_Sub1C580(uint32_t size, const char* tag, int32_t line, uint32_t flags);            // 0x1C580
extern "C" int32_t Storm_Sub1B960(void* p, const char* tag, int32_t line);                               // 0x1B960

static const char kPlayerName[] = "AU_PLAYERNAME";   // retail .rdata 0x51DA4

extern "C" EMSCRIPTEN_KEEPALIVE void Storm_Sub5440(void* tbl, uint32_t n) {
  uint32_t* w = (uint32_t*)tbl;                                  // retail dwords: w[0] count, w[1] oldCount, w[2] data
  uint8_t* old = (uint8_t*)(uintptr_t)w[2];
  w[0] = n;
  uint8_t* p = (uint8_t*)Storm_SMemReAlloc(old, n << 7, kPlayerName, -2, 0x10);
  if (p) { w[2] = (uint32_t)(uintptr_t)p; return; }
  uint8_t* fresh = (uint8_t*)Storm_Sub1C580(n << 7, kPlayerName, -2, 0);
  w[2] = (uint32_t)(uintptr_t)fresh;
  if (!old) return;
  uint32_t c = w[1] <= n ? w[1] : n;
  if (fresh) for (uint32_t i = 0; i < c; i++) std::memcpy(fresh + (i << 7), old + (i << 7), 128);
  Storm_Sub1B960(old, kPlayerName, -2);
}
