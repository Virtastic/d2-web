// 1.13c D2Game+0x5D960 (EBX = object GUID, stack: quest, quest arg; ret 8): spawn missile 541 from the quest arg's target to the object with that GUID.
// Object list = game+0x1520 hash (GUID & 0x7F), chained through +0xE4; no unit-type check. The 23-dword request goes to 0x6F930 (ECX game, EDX request).
#include "d2game_q32.h"
using namespace q32;
// 1.13c D2Game+0x5D960
void d2game_5d960_q32(const Call& X, uint32_t id, uint32_t quest, uint32_t qarg)
{
    uint32_t game = rd32(quest, 4);
    if (!game) __builtin_trap();   // retail: D2_ASSERT text then _exit(-1)
    uint32_t u = rd32(game, 0x1520 + (id & 0x7f) * 4);
    while (u && rd32(u, 0xc) != id) u = rd32(u, 0xe4);
    if (!u) return;
    uint32_t m[23] = {};
    const uint32_t mp = (uint32_t)(uintptr_t)m;
    const uint32_t tgt = rd32(qarg, 8);
    wr32(mp, 0, 0x420); wr32(mp, 4, tgt); wr32(mp, 8, tgt); wr32(mp, 0x10, 0x21d);
    const int32_t type = (int32_t)rd32(u, 0);
    const uint32_t path = rd32(u, 0x2c);
    const bool direct = type == 2 || (type > 3 && type < 6);
    wr32(mp, 0x1c, direct ? rd32(path, 0xc) : path ? X(0xA454, 0, 0, 0, 0, 0, path) : 0);
    wr32(mp, 0x20, direct ? rd32(path, 0x10) : path ? X(0xA3B8, 0, 0, 0, 0, 0, path) : 0);
    wr32(mp, 0x30, 1);
    uint32_t mis = X(0x6F930, 0, game, mp);
    if (mis) X(0xA50E, 0, 0, 0, 0, 0, mis, id);
}
