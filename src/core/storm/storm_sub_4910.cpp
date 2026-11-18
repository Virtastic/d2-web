// src/core/storm/storm_sub_4910.cpp -- 1.13c Storm+0x04910
#include <cstdint>
// Destructor of a 0x3068+ object: member dtors at +0x305C then +0x3050, then the 0x203-element
// array of 0x18-byte entries at +8 destroyed (reverse order, per _eh_vector_destructor_iterator_).
struct StormSub4910Env {
  void (*memberDtor)(uint8_t* m);   // Storm+0x4520
  void (*elemDtor)(uint8_t* e);     // Storm+0x47D0
};
// 1.13c Storm+0x04910
extern "C" void Storm_Sub4910(const StormSub4910Env& e, uint8_t* self) {
  e.memberDtor(self + 0x305C);
  e.memberDtor(self + 0x3050);
  for (int i = 0x203; i-- > 0;) e.elemDtor(self + 8 + i * 0x18);
}
