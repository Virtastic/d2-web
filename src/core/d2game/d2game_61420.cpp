// 0x61420 (EAX = game, EDX = unit or 0; plain ret): first type-4 quest node; removes the unit's id (-1 if none) from the list at data (+0x18) +0xB4 (0x33830).
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x61420
uint32_t d2game_61420_q32(const Call& X, uint32_t game, uint32_t unit) {
    uint32_t n = findQ(game, 4);
    uint32_t id = unit ? rd32(unit, 0xc) : 0xffffffffu;
    return X(0x33830, id, rd32(n, 0x18) + 0xb4);
}
