// 1.13c Fog+0xFD70 -- EDX = value: calls the cdecl worker 0x190A0 with (the module string at image offset 0x268EC, value) and returns its result.
#include <cstdint>
#include "../fn/d2game_gp.h"
namespace fg {
uint32_t ffd70(GE& e, uint32_t, uint32_t d)
{
    return e.h(0x190A0, 0, 0, 0, 0, 0, 0xB20268ECu, d);
}
}
