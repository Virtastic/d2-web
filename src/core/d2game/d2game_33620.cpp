// Object init (Vile Dog afterglow): an object with no unit or no +0x10 data gets mode 1 (D2Common #11090) and two
// events (0x8EA80): kind 7 at frame+0x1B, kind 1 at frame + 1 + (objects.txt record +0xDC >> 8).
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G33620Env {
    void (*setMode)(uint32_t unit, uint32_t mode);   // D2Common #11090 (thunk 0xA4A2), stdcall
    void (*event)(uint32_t game, uint32_t unit, uint32_t kind, uint32_t frame, uint32_t a, uint32_t b, uint32_t c);   // 0x8EA80: ECX game, EDX unit
};

// 1.13c D2Game+0x33620
void OBJECTS_InitVileDogAfterglow(uint32_t op, const D2G33620Env& env)
{
    const uint32_t unit = rd32(op + 4);
    if (unit && rd32(unit + 0x10)) return;
    env.setMode(unit, 1);
    env.event(rd32(op), rd32(op + 4), 7, rd32(rd32(op) + 0xA8) + 0x1B, 0, 0, 0);
    env.event(rd32(op), rd32(op + 4), 1, (rd32(rd32(op + 0x10) + 0xDC) >> 8) + 1 + rd32(rd32(op) + 0xA8), 0, 0, 0);
}
