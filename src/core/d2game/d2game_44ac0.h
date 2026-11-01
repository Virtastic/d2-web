// 1.13c D2Game+0x44AC0 (druid melee-hit helper: sum the to-hit of every transform state, then allocate the combat),
// transcribed from the 1.13c disassembly. Calling convention: stdcall(game, unit, level), ret 0xC. Every callee is one
// E::h(rva, eax, ecx, edx, ebx, esi, stack words...) call exactly as retail passes it; the damage struct is the
// 0x70-byte stack local that EDI addresses for 0xDDDE0 (its first 36 bytes are logged by edi9).
#pragma once
#include <cstdint>

namespace r44ac0 {
template <class E> uint32_t Do(E& e, uint32_t dt, uint32_t game, uint32_t unit, uint32_t level)
{
    e.h(0xE07A0, unit, game);
    const uint32_t tgt = e.h(0xA598, 0, 0, 0, 0, 0, e.rd32(unit + 0x2C));
    if (tgt == unit || !tgt) return 0;
    uint32_t sum = 0;
    uint8_t flag = 0x80;
    for (int32_t i = 0; i < e.rd16s(dt + 0x180); i++) {
        const int32_t st = e.rd16s(e.rd32(dt + 0x17C) + 2 * i);
        if (!e.h(0xA68E, 0, 0, 0, 0, 0, unit, (uint32_t)st)) continue;
        if (st < 0 || st >= (int32_t)e.rd32(dt + 0xC4) || (uint32_t)st * 0x3C + e.rd32(dt + 0xBC) == 0) continue;
        const uint32_t sk = e.h(0xA5D4, 0, 0, 0, 0, 0, unit, (uint32_t)st);
        if (!sk) continue;
        const int32_t id = (int32_t)e.h(0xA712, 0, 0, 0, 0, 0, sk, 0x15E, 0);
        if (id < 0 || id >= (int32_t)e.rd32(dt + 0xBA0)) continue;
        sum += e.h(0xACD6, 0, 0, 0, 0, 0, unit, (uint32_t)id, level);
        const uint32_t rec = e.h(0x1420, (uint32_t)id);
        if (rec && e.rd8(rec + 0x1A5)) flag = (uint8_t)e.rd8(rec + 0x1A5);
    }
    uint32_t dmg[0x1C] = {};
    dmg[1] = e.h(0xDE5A0, tgt, 0, 0, 0, 0, game, sum, 0) & 0xFFFF;
    dmg[0x18] = 1;
    e.h(0xDDDE0, unit, flag, 0, 0, 0, game, tgt);
    e.edi9(0xDDDE0, dmg);
    return 1;
}
}
