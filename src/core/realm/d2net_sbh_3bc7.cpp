// D2Net small-block heap: ___sbh_alloc_new_region (0x3BC7) and ___sbh_alloc_new_group (0x3C7E), MSVC 2003 CRT.
// Same bodies as D2MCPClient 0x3887/0x393E, written over host pointers; Win32 calls are seams.
#include <cstdint>
#include <cstring>
#include <cstdlib>

namespace d2net_sbh {
struct Heap { uint32_t count, capacity; uint8_t* regions; };  // DAT_6fbfb43c / 44c / 440
struct Env {
  void* (*realloc_)(void*, size_t);
  void* (*zalloc)(size_t);
  void (*free_)(void*);
  void* (*valloc)(void* addr, size_t n);  // MEM_RESERVE when addr null, MEM_COMMIT otherwise
};

// 1.13c D2Net+0x3BC7
uint32_t* sbh_alloc_new_region(Heap& h, const Env& e) {
  if (h.count == h.capacity) {
    void* p = e.realloc_(h.regions, (h.capacity * 5 + 0x50) * 4);
    if (!p) return nullptr;
    h.capacity += 0x10;
    h.regions = (uint8_t*)p;
  }
  uint32_t* r = (uint32_t*)(h.regions + h.count * 0x14);
  void* hdr = e.zalloc(0x41c4);
  std::memcpy(&r[4], &hdr, sizeof hdr);  // retail stores a 32-bit pointer; host keeps it beside the record
  if (!hdr) return nullptr;
  void* mem = e.valloc(nullptr, 0x100000);
  if (!mem) { e.free_(hdr); return nullptr; }
  r[2] = 0xffffffffu; r[0] = r[1] = 0;
  h.count++;
  *(uint32_t*)hdr = 0xffffffffu;
  return r;
}

// 1.13c D2Net+0x3C7E  region: {[1] flags, [2] free-group mask, base, ctl}; offsets as retail
int sbh_alloc_new_group(uint32_t* region, uint8_t* ctl, uint8_t* base, const Env& e) {
  int g = 0;
  for (int32_t m = (int32_t)region[2]; m >= 0; m <<= 1) g++;
  uint8_t* b = ctl + g * 0x204 + 0x144;
  for (int i = 0; i < 0x3f; i++) { uint8_t** n = (uint8_t**)(b + i * 8); n[1] = n[2] = (uint8_t*)n; }
  uint8_t* a = base + g * 0x8000;
  if (!e.valloc(a, 0x8000)) return -1;
  for (int i = 0; i < 8; i++) {
    uint8_t* p = a + 0x10 + i * 0x1000;
    ((int32_t*)p)[-2] = -1; ((int32_t*)p)[0x3fb] = -1;
    ((uint8_t**)p)[0] = p + 0xffc; ((int32_t*)p)[-1] = 0xff0;
    ((uint8_t**)p)[1] = p - 0x1004; ((int32_t*)p)[0x3fa] = 0xff0;
  }
  ((uint8_t**)(b + 0x1fc))[0] = a + 0xc;  *(uint8_t**)(a + 0x14) = b + 0x1f8;
  ((uint8_t**)(b + 0x200))[0] = a + 0x700c; *(uint8_t**)(a + 0x7010) = b + 0x1f8;
  ((uint32_t*)(ctl + 0x44))[g] = 0; ((uint32_t*)(ctl + 0xc4))[g] = 1;
  if (ctl[0x43]++ == 0) region[1] |= 1;
  region[2] &= ~(0x80000000u >> (g & 31));
  return g;
}
}  // namespace d2net_sbh
