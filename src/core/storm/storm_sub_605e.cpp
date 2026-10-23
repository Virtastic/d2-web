// src/core/storm/storm_sub_605e.cpp -- 1.13c Storm+0x0605E
#include <cstdint>
// 1.13c Storm+0x0605E: MSVC `eh vector destructor iterator'(ptr,size,count,dtor): runs dtor on elements
// count-1..0 (reverse order). The SEH frame / 0x60A6 epilogue is unwind plumbing, no data effect.
extern "C" void Storm_Sub605E(uint8_t* p, uint32_t size, int32_t count, void (*dtor)(void*)) {
  p += (uint64_t)size * (uint32_t)count;
  while (--count >= 0) { p -= size; dtor(p); }
}
