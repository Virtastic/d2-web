// Frozen Orb client missiles (Skills.txt 64, Missiles.txt 260 frozenorb / 261 frozenorbbolt / 262 frozenorbnova): the orb's do
// function 0xB8840 (pCltDoFunc 19), its hit function 0xB6010 (pCltHitFunc 30) and the nova's do function 0xB29B0 (pCltDoFunc 20).
// Each takes the engine's child creator (clmissile_b17c0.cpp, 0xB30F0 with request flags 0x2): the missile it made, or null.
#pragma once
#include <cstdint>
#include <Units/Units.h>

namespace d2client_clmissile_orb {
using Make = D2UnitStrc* (*)(D2UnitStrc* owner, int32_t missileId, int32_t skillId, int32_t level, int32_t x, int32_t y, int32_t tx, int32_t ty);
extern const int32_t kCos[64];   // 1.13c D2Client+0xD3F10; the sine table 0xD3E10 is the same table 16 slots later
extern const int32_t kSin[64];
void phaseSpawn(D2UnitStrc* unit, D2UnitStrc* owner, Make make);   // 1.13c D2Client+0xB8840 (before the 0xB17C0 tail)
void ring(D2UnitStrc* unit, D2UnitStrc* owner, Make make);          // 1.13c D2Client+0xB6010
void novaMove(D2UnitStrc* unit);                                    // 1.13c D2Client+0xB29B0 (before the 0xB17C0 tail)
}
