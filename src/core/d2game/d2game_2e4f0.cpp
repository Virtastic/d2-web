// OBJECTS_SetUnitIdInTimerArg: store a unit id in the timer-argument block of an object (ECX = object, stack = id, ret 4).
#include "d2game_2e790.h"
using namespace d2g32;

// 1.13c D2Game+0x2E4F0; timerArgs = D2Common #10181 (thunk 0xB168, ECX = object)
void OBJECTS_SetUnitIdInTimerArg(uint32_t object, uint32_t unitId, uint32_t (*timerArgs)(uint32_t object))
{
    const uint32_t arg = timerArgs(object);
    if (arg) wr32(arg + 4, unitId);
}
