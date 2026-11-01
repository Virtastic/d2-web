// Declarations for d2client_30270.cpp (the item-description tooltip tail), shared with its FN probe in src/core/fn.
#pragma once
#include <cstdint>
namespace d2client_30270 {
using U = uint32_t; using I = int32_t;
struct Ops {
 U (*o2F2E0)(U eax, U ecx, U edx, U a0, U a2);          // returns eax
 void (*o2EF80)(U eax, U ebx, U ecx, U esi, U a0, U a2); // stack esi,a0,a2,0
 void (*o2D000)(U eax, U ebx, U edx, U esi, U a0, U a1); // stack esi,a0,a1,0
 U (*oCEAA)(U esi, U a1, U edi);                        // import thunk, stdcall
 void (*o2D9B0)(U eax, U ebx, U v);                     // stack 0x10a6,0x10b3,v,1
 void (*o2E190)(U eax, U ebx, U esi, I* mn, I* mx, I* mode);
 void (*o2F520)(U eax, I mn, I mx);                     // stack 0x10a0,mn,mx,0,0
};
// eax=edi, esi register args, stack a0,a1,a2 (ret 0xc); `flag` is the global at D2Client+0x119854
U sub_30270(const Ops& o, bool flag, U edi, U esi, U a0, U a1, U a2);
}
