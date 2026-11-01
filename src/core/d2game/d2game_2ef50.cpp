// Object init: the shared init routine (0x2ECB0), then the object's byte at +0x78 := 1 (a missing object is fatal).
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G2EF50Env {
    void (*initFire)(uint32_t op);   // 0x2ECB0: ESI = op
    void (*fatal)(uint32_t line);
};

// 1.13c D2Game+0x2EF50
void OBJECTS_InitFireAndFlag(uint32_t op, const D2G2EF50Env& env)
{
    env.initFire(op);
    if (rd32(op + 4) == 0) { env.fatal(0xA48); return; }
    wr8(rd32(op + 4) + 0x78, 1);
}
