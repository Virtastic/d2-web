// 1.13c D2Game+0x15890 (AI unit-scan callback, minion spawner: counts live non-allied class 0x1C5 / 0x1CD monsters):
// EDX = scanning unit, stack = (target, &counter) ret 8; ECX unused. Written from the 1.13c disassembly as a rule over E (see d2game_d040.h).
// Rule: nothing happens for the unit itself, a null target or a non-monster (type != 1). 0xA688 (stdcall target) nonzero: skip.
// Class 0x18A0 (eax = target; plain ret), -1 outside 0..monstats count (dt+0xA80); only 0x1C5 or 0x1CD continue. 0xA514 (stdcall
// target) == 0 increments *counter. The result is always 0.
#pragma once
#include <cstdint>

namespace g15890 {
template <class E> uint32_t MinionSpawnerCallback(E& e, uint32_t dt, uint32_t src, uint32_t tgt, uint32_t counter)
{
    // 1.13c D2Game+0x15890
    if (src == tgt || !tgt || e.rd32(tgt) != 1) return 0;
    if (e.h(0xA688, 0, 0, 0, 0, 0, tgt)) return 0;
    int32_t c = (int32_t)e.h(0x18A0, tgt);
    if (c < 0 || c >= (int32_t)e.rd32(dt + 0xA80)) c = -1;
    if (c != 0x1C5 && c != 0x1CD) return 0;
    if (e.h(0xA514, 0, 0, 0, 0, 0, tgt) == 0) e.wr32(counter, e.rd32(counter) + 1);
    return 0;
}
}
