// 1.13c D2Game+0x36820 QUESTS_SpawnMonster: maps the super-unique id, finds the room under the
// position and spawns a preset monster there. EAX = nIndex, ECX = nType, ESI = pPosition, stack = pGame, pUnit (ret 8).
// Delta against the vendored 1.10f: none in the rule; the 1.13c entry is register-convention (see the spec).
#include <cstdint>

struct D2Game36820Env {
    uint32_t (*mapSuper)(uint32_t nType, uint32_t nIndex);                         // D2Common #10285 via 0xADF6
    uint32_t (*unitRoom)(uint32_t unit);                                           // 0xA478
    uint32_t (*findRoom)(uint32_t room, int32_t x, int32_t y);                     // 0x1280 (EAX room, EBX x, EDI y)
    uint32_t (*spawn)(uint32_t classId, uint32_t room, uint32_t game, int32_t x, int32_t y, uint32_t mode);   // 0x47CF0
};

uint32_t D2Game_36820(uint32_t nIndex, uint32_t nType, const int32_t* pPos, uint32_t pGame, uint32_t pUnit, const D2Game36820Env& e)
{
    const uint32_t id = e.mapSuper(nType, nIndex);
    const uint32_t room = e.unitRoom(pUnit);
    const int32_t x = pPos[0], y = pPos[1];
    const uint32_t found = e.findRoom(room, x, y);
    if (!found) return 0;
    return e.spawn(id, found, pGame, x, y, 1);
}
