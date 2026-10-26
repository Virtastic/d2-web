#include <cstdint>
// Unwind (SEH cleanup) funnels; ebp-frame arg is the owning object (32-bit link). Seams are retail callees.
void z28_Storm_4C20();                        // Storm+0x4C20
void z28_Storm_4B80(uint32_t* p);             // Storm+0x4B80
void z28_Storm_53C0(void* p);                 // Storm+0x53C0
void z28_Storm_eh_vec_dtor(void* base, uint32_t size, uint32_t n, void (*dtor)(void*)); // Storm+0x605E
static inline uint8_t* z28_obj(uint32_t* ebp) { return (uint8_t*)(uintptr_t)ebp[1]; }
static void z28_vec(uint32_t* ebp, uint32_t off) { z28_Storm_eh_vec_dtor(z28_obj(ebp) + off, 0xc, 4, z28_Storm_53C0); }
// 1.13c Storm+0x41A30
void z28_Unwind_41A30() { z28_Storm_4C20(); }
// 1.13c Storm+0x41A50
void z28_Unwind_41A50(uint32_t* ebp) { z28_Storm_4B80((uint32_t*)(uintptr_t)ebp[1]); }
// 1.13c Storm+0x41A5A
void z28_Unwind_41A5A(uint32_t* ebp) { z28_vec(ebp, 0x130); }
// 1.13c Storm+0x41A72
void z28_Unwind_41A72(uint32_t* ebp) { z28_vec(ebp, 0x160); }
// 1.13c Storm+0x41A8A
void z28_Unwind_41A8A(uint32_t* ebp) { z28_vec(ebp, 400); }
// 1.13c Storm+0x41AA2
void z28_Unwind_41AA2(uint32_t* ebp) { z28_Storm_53C0(z28_obj(ebp) + 0x1c0); }
// 1.13c Storm+0x41AC8
void z28_Unwind_41AC8(uint32_t* ebp) { z28_Storm_4B80((uint32_t*)(uintptr_t)ebp[1]); }
// 1.13c Storm+0x41AD2
void z28_Unwind_41AD2(uint32_t* ebp) { z28_vec(ebp, 0x130); }
// 1.13c Storm+0x41AEA
void z28_Unwind_41AEA(uint32_t* ebp) { z28_vec(ebp, 0x160); }
// 1.13c Storm+0x41B02
void z28_Unwind_41B02(uint32_t* ebp) { z28_vec(ebp, 400); }
// 1.13c Storm+0x41B1A
void z28_Unwind_41B1A() { z28_Storm_4C20(); }
