// Object init: in level 1 one event (0x8EA80) kind 8 at frame+0x3C; elsewhere mode 2 (D2Common #11090) and kind 0
// at frame+0x19. The level comes from D2Common #10826 on op+8.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G33740Env {
    uint32_t (*levelId)(uint32_t room);              // D2Common #10826 (thunk 0xA556)
    void (*setMode)(uint32_t unit, uint32_t mode);   // D2Common #11090 (thunk 0xA4A2)
    void (*event)(uint32_t game, uint32_t unit, uint32_t kind, uint32_t frame, uint32_t a, uint32_t b, uint32_t c);   // 0x8EA80
};

// 1.13c D2Game+0x33740
void OBJECTS_InitLevelOneEvent(uint32_t op, const D2G33740Env& env)
{
    if (env.levelId(rd32(op + 8)) == 1) {
        env.event(rd32(op), rd32(op + 4), 8, rd32(rd32(op) + 0xA8) + 0x3C, 0, 0, 0);
        return;
    }
    env.setMode(rd32(op + 4), 2);
    env.event(rd32(op), rd32(op + 4), 0, rd32(rd32(op) + 0xA8) + 0x19, 0, 0, 0);
}
