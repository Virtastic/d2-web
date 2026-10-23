#include <cstdint>
void sub_4B80(intptr_t* p);
// 1.13c Storm+0x3630: zero a 5-dword element (fastcall ecx = element).
void Storm_Sub3630(uint32_t* e) { e[0] = e[1] = e[2] = e[3] = e[4] = 0; }
// 1.13c Storm+0x5370: 12-byte list-head ctor; e[1] points at itself, e[2] = ~&e[1], e[0] = 0 (32-bit pointers).
void Storm_Sub5370(uint32_t* e) { e[1] = (uint32_t)(uintptr_t)&e[1]; e[0] = 0; e[2] = ~e[1]; }
// 1.13c Storm+0x47D0: element dtor (SEH frame only); unlinks via Storm+0x4B80.
void Storm_Sub47D0(intptr_t* e) { sub_4B80(e); }
