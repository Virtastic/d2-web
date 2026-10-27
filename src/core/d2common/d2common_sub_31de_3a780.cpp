#include <cstdint>
#include "D2StatList.h"

struct D2CommonUnwindEnv;
void d2common_local_unwind2_5fd6(const D2CommonUnwindEnv& e, uint8_t* frame, int32_t target);

// 1.13c D2Common+0x031DE sub_31DE: SEH cleanup thunk. param is a retail 32-bit exception-registration
// record; dwords at +0x18/+0x1c are the frame and target handed to __local_unwind2 (0x5FD6).
void d2common_31de(const D2CommonUnwindEnv& e, uint8_t* rec)
{
    d2common_local_unwind2_5fd6(e, (uint8_t*)(uintptr_t) * (uint32_t*)(rec + 0x18), *(int32_t*)(rec + 0x1c));
}

// 1.13c D2Common+0x3A780 sub_3A780: register shim, EAX = layer, ECX = value, EDX = stat id, one stack word = list
// -> 0x3A280 STATLIST_SetStat(list, statId, value, layer); the result passes through (ret 4).
BOOL D2Common_3A780_SetStatRegs(uint32_t layer, int value, int statId, D2StatListStrc* list)
{
    return STATLIST_SetStat(list, statId, value, (uint16_t)layer);
}
