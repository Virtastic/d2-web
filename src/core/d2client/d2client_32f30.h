// Declarations for d2client_32f30.cpp (the skill-description damage range for a unit's skill or item), shared with its FN probe in src/core/fn.
#pragma once
#include <cstdint>
namespace d2client_32f30 {
using U = uint32_t; using I = int32_t; using P = void*;
struct Ops {
 P (*owner)(P unit);                              // unit+0x60
 P (*skillOf)(P own);                             // D2Client+0xCA0C
 I (*oC76C)(P item);
 I (*oC1CC)(P item, I k);
 U (*oC736)(P item);
 I (*oC880)(I z, P unit, U e, I one); I (*oC8E0)(I z, P unit, U e, I one);
 I (*oC934)(I z, P unit, U e, I one); I (*oC96A)(I z, P unit, U e, I one);
 uint8_t (*oC868)(U e);
 I (*oC832)(I z, P unit, U e, I one);
 int16_t (*oC940)(P item); int16_t (*oC8CE)(P item);
 I (*stat)(P unit, I id, I layer);               // Ordinal_10973
 I (*oC1FC)(P unit, I id);
 I (*oC922)(P unit, P item, U skill, I one);
 void (*o30E30)(P unit, I* mn, I* mx, I* mode, U flag);
};
// ecx=unit, edx=pMin; stack pMax, pMode, a2, a3, skill, a5, item (ret 0x1c); a3 and a5 are never read
void sub_32F30(const Ops& o, P unit, I* pMin, I* pMax, I* pMode, I a2, U skill, P item);
}
