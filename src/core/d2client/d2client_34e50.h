// Declarations for d2client_34e50.cpp (the skill-description bonus range with item and stat percent scaling), shared with its FN probe in src/core/fn.
#pragma once
#include <cstdint>
namespace d2client_34e50 {
using U = uint32_t; using I = int32_t; using P = void*;
struct Ops {
 U (*o2C830)(U v);                                              // eax in, eax out
 void (*o34D70)(U* pMode, U ecx, U esi, U edi, P unit, I* x, I* y);
 I (*oC6EE)(P unit, I one);
 I (*oC89E)(P unit, P own);
 I (*stat)(P unit, I id, I layer);                              // Ordinal_10973
 I (*oC316)(P unit, U field, I id, I b);
 void (*emit)(I eax, I ecx, U mode, U w2, U w3, U w4);          // D2Client+0x2F0A0
};
// ecx=unit, edx=a; stack rec, b, w2, w3, w4 (ret 0x14)
void sub_34E50(const Ops& o, P unit, U a, const uint8_t* rec, I b, U w2, U w3, U w4);
}
