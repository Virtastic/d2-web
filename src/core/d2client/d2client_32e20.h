// Declarations for d2client_32e20.cpp (the skill-description percent scaling pass), shared with its FN probe in src/core/fn.
#pragma once
#include <cstdint>
namespace d2client_32e20 {
using U = uint32_t; using I = int32_t;
struct Ops {
 void (*o31220)(U unit, U flag, I* mn, I* mx, I* mode);     // eax=0, ecx=unit, stack mn,mx,mode,0,flag,0,0,0
 I (*oC42A)(U key, U rva, I line);                           // import thunk, stdcall (key, string at D2Client+rva, line)
 void (*o30E30)(U unit, I* mn, I* mx, I* mode, U flag);
 void (*emit)(I eax, I ecx, I s0, I s1, I s2, I s3);         // D2Client+0x2F0A0
};
// ecx=unit, edx=flag, stack a0..a4 (ret 0x14); dt = the data-table struct reached through the pointer global at D2Client+0xCE474
void sub_32E20(const Ops& o, const uint8_t* dt, U unit, U flag, U a0, U a2, U a3, U a4);
}
