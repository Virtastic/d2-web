// Declarations for d2client_35140.cpp and d2client_35270.cpp (skill-description damage ranges built from the skill's
// min/max damage, the 0x34D70 bonus pass and the poison-length or elemental terms), shared with their FN probes in src/core/fn.
#pragma once
#include <cstdint>
namespace d2client_35140 {
using U = uint32_t; using I = int32_t; using P = void*;
struct Ops {
 U (*o2C830)(U v);                                              // eax in, eax out
 void (*o34D70)(U* pMode, U ecx, U esi, U edi, P unit, I* x, I* y);
 I (*dmgMin)(P unit, I skill, I lvl, I z);                      // import thunk 0xC85C
 I (*dmgMax)(P unit, I skill, I lvl, I z);                      // import thunk 0xC7F0
 I (*len)(P unit, I skill, I lvl, I one);                       // import thunk 0xCEB6
 I (*eMin)(P unit, I skill, I lvl, I one);                      // import thunk 0xCEC2
 I (*eMax)(P unit, I skill, I lvl, I one);                      // import thunk 0xCECE
 void (*emit)(I eax, I ecx, U mode, U w2, U w3, U w4);          // D2Client+0x2F0A0
};
// ecx=unit, edx=flag; stack rec, lvl, w2, w3, w4 (ret 0x14)
void sub_35140(const Ops& o, P unit, U flag, const uint8_t* rec, I lvl, U w2, U w3, U w4);
void sub_35270(const Ops& o, P unit, U flag, const uint8_t* rec, I lvl, U w2, U w3, U w4);
}
