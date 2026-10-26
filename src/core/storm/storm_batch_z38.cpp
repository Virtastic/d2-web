#include <cstdint>
// SEH unwind funclets. Each reads the owning object from [ebp+4]; here the caller passes it
// (`self`, 32-bit link) or the frame base (`ebp`). Callees are seams.
void z38_Storm_Sub53C0(uint32_t p);
void z38_Storm_Sub4C20();
void z38_Storm_Sub47D0(uint32_t e);
void z38_Storm_Sub4520(uint32_t p);
void z38_Storm_Sub4910(uint32_t p);
void z38_Storm_Sub4B40(uint32_t p);
// _eh_vector_destructor_iterator_: destroy count elements back to front.
static void z38_vec_dtor(uint32_t base, uint32_t size, uint32_t count) {
  while (count--) z38_Storm_Sub47D0(base + count * size);
}
// 1.13c Storm+0x41B25
void z38_Unwind_41B25(uint32_t self) { z38_Storm_Sub53C0(self + 0x1c0); }
// 1.13c Storm+0x41B40
void z38_Unwind_41B40() { z38_Storm_Sub4C20(); }
// 1.13c Storm+0x41B60
void z38_Unwind_41B60(uint32_t self) { z38_vec_dtor(self + 8, 0x18, 0x203); }
// 1.13c Storm+0x41B79
void z38_Unwind_41B79(uint32_t self) { z38_Storm_Sub4520(self + 0x3050); }
// 1.13c Storm+0x41B88
void z38_Unwind_41B88(uint32_t self) { z38_Storm_Sub4520(self + 0x305c); }
// 1.13c Storm+0x41BB0
void z38_Unwind_41BB0(uint32_t self) { z38_vec_dtor(self + 8, 0x18, 0x203); }
// 1.13c Storm+0x41BC9
void z38_Unwind_41BC9() { z38_Storm_Sub4C20(); }
// 1.13c Storm+0x41BD4
void z38_Unwind_41BD4(uint32_t self) { z38_Storm_Sub4520(self + 0x3050); }
// 1.13c Storm+0x41BE3
void z38_Unwind_41BE3() { z38_Storm_Sub4C20(); }
// 1.13c Storm+0x41BEE
void z38_Unwind_41BEE(uint32_t self) { z38_Storm_Sub4520(self + 0x305c); }
// 1.13c Storm+0x41C07
void z38_Unwind_41C07(uint32_t self) { z38_Storm_Sub4910(self); }
// 1.13c Storm+0x41C1B (arg is the frame base ebp)
void z38_Unwind_41C1B(uint32_t ebp) { z38_Storm_Sub4B40(ebp - 0x3a80); }
