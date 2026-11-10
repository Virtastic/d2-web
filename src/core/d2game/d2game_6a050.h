// 1.13c D2Game+0x6A050: quest event dispatch by index. EBX = index, ESI = {game, unit},
// one stack word (ret 4). Callees go through a HookEnv: 0xA478 (#10331 GetRoom), 0xA556 (#10826 level id), 0xA56E (#10459 level test),
// 0xEDF20 (stdcall 5: game, index, stack word, mode, 0; EDI = the unit, logged as a J record).
#pragma once
#include <cstdint>
#include "../fn/hookenv.h"
namespace g6a050 {
// 1.13c D2Game+0x6A050. `count` is the dword at sgptDataTables+0xA80.
inline void QuestEventDispatch(HookEnv& e, int32_t index, uint32_t esi, uint32_t arg, uint32_t count)
{
    if (index < 0 || (int32_t)count <= index) return;   // signed compare (jl / jge)
    const uint32_t game = HookEnv::rd32(esi), unit = HookEnv::rd32(esi + 4);
    uint32_t room = e.h(0xA478, 0, 0, 0, 0, 0, unit);
    if (index == 0xEA && room != 0) {
        uint32_t lvl = e.h(0xA556, 0, 0, 0, 0, 0, room);
        if ((uint8_t)e.h(0xA56E, 0, 0, 0, 0, 0, lvl) == 0) return;
    }
    static const uint32_t mode[3] = { 0xFFFFFFFFu, 1, 3 };
    for (int i = 0; i < 3; i++) {
        const uint32_t s4 = 0;   // push 0; later calls push eax, which is 0 there
        e.ed(unit);
        uint32_t r = e.h(0xEDF20, 0, 0, 0, 0, 0, game, (uint32_t)index, arg, mode[i], s4);
        e.edi(0xEDF20, unit);
        if (r) return;
    }
}
}
