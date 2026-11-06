// 1.13c D2Game+0x68F90 -- RaiseQuestNodeFlagAndSend. ECX = p {+0 game, +4 unit}. Written from the 1.13c disassembly.
// 0xA4A2 (D2Common ordinal 11090 through its thunk; stack unit, 1), then 0x8EA80 with ECX = game, EDX = unit and five
// stack words 7, game->+0xA8 + 0x19, 0, 0, 0. The result is the last callee's EAX.
#include "../fn/hookenv.h"
namespace d2g {
uint32_t f68f90(HookEnv& e, uint32_t p)
{
    const uint32_t game = e.rd32(p), unit = e.rd32(p + 4);
    e.h(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
    return e.h(0x8EA80, 0, game, unit, 0, 0, 7, e.rd32(game + 0xA8) + 0x19, 0, 0, 0);
}
}
