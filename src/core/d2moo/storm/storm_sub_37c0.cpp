// src/core/storm/storm_sub_37c0.cpp -- 1.13c Storm+0x37C0
#include <cstdint>
struct StormSub37C0Env {
  void (*elemCtor)(void* elem);  // Storm+0x3630, run on each of 16 elements
};
// 1.13c Storm+0x37C0: construct 16 x 0x14-byte elements, clear dword at +0x140, return this.
// (The EH unwind dtor 0x3820 only runs if a ctor throws; not modelled.)
extern "C" void* Storm_Sub37C0(const StormSub37C0Env& e, void* self) {
  uint8_t* p = (uint8_t*)self;
  for (int i = 0; i < 16; i++) e.elemCtor(p + i * 0x14);
  *(uint32_t*)(p + 0x140) = 0;
  return self;
}
