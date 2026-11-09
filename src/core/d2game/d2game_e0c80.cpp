// 1.13c D2Game+0xE0C80, from the disassembly (jump table at +0xE0CE8: kind 0 runs the slot release then a6ee, kinds 1..3 a6ee,
// others go straight on). p1 == 0 adds a63a(unit). Always ends in finish(unit, game).
#include "d2game_e0c80.h"
#include <cstring>
void d2game_e0c80_port(uint32_t unit, uint32_t game, uint32_t p1, const D2GameE0C80Env* e) {
    if (!unit) e->fatal(0x3ad);
    uint32_t kind; std::memcpy(&kind, (const void*)(uintptr_t)unit, 4);
    if ((int32_t)kind >= 6) e->fatal(0x3ae);
    if (kind == 0) e->slotRelease(unit, game);
    if (kind <= 3) e->a6ee(unit);
    if (!p1) e->a63a(unit);
    e->finish(unit, game);
}
