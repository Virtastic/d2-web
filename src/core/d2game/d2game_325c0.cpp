// 1.13c D2Game+0x325C0 OBJECTS_SpawnShrine, retail 32-bit layout, every engine callee an Env member.
// ECX = pGame, EDX = pRoom, stack = nClassId, nX, nY, nMode (ret 0x10). Classes 574..579 index a 6-row {objectId, min, max} table
// (module data at RVA 0xFCCC0); the shrine id is roll(max - min) + min, 4 and 5 fold to 2. Delta against the 1.10f D2MOO function:
// none in the rule; SUNIT_AllocUnitData is inlined here (same field writes as the barrel spawner 0x2FEC0).
#include <cstdint>
#include "d2game_2e790.h"

struct D2G325C0Env {
    uint32_t (*tableRow)(uint32_t idx);                 // address of the table row (idx 0..5)
    uint32_t (*allocUnit)(uint32_t pool, uint32_t type);                   // 0xA6B8
    uint32_t (*roomLevel)(uint32_t room);                                  // 0xA556
    uint32_t (*levelAct)(uint32_t level);                                  // 0xA56E
    void (*initUnit)(uint32_t unit, uint32_t gameBlock);                   // 0xE0110
    uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);        // 0xA2E6
    uint32_t (*objTxt)(uint32_t objId);                                    // 0xA520
    void (*setUnitState)(uint32_t unit);                                   // 0xA724
    void (*place)(uint32_t unit, uint32_t id, uint32_t room, uint32_t x, uint32_t y);          // 0x2F300
    void (*attach)(uint32_t room, uint32_t y, uint32_t unit, uint32_t x, uint32_t game, uint32_t one);   // 0xE0F40
    uint32_t (*roll)(uint32_t seed, uint32_t range);                       // 0x11D0
    uint32_t (*shrineTxt)(uint32_t id);                                    // 0xB2CA
    void (*setShrine)(uint32_t unit, uint32_t rec);                        // 0xB2EE
    void (*fatal)();
};

uint32_t OBJECTS_SpawnShrine(uint32_t game, uint32_t room, int32_t cls, uint32_t x, uint32_t y, uint32_t mode, const D2G325C0Env& e)
{
    using namespace d2g32;
    if (cls < 0x23e || cls > 0x243) return 0;
    if (!room) e.fatal();
    const uint32_t row = e.tableRow((uint32_t)(cls - 0x23e));
    const uint32_t objId = rd32(row);
    const uint32_t pool = rd32(game + 0x1c);
    const uint32_t u = e.allocUnit(pool, 2);
    if (!u) e.fatal();          // retail falls through and faults on the null unit
    wr32(u + 0, 2); wr32(u + 4, objId); wr32(u + 8, pool); wr32(u + 0x80, game);
    uint32_t fl = rd32(u + 0xc8);
    fl = rd32(game + 0x70) ? (fl | 0x2000000u) : (fl & 0xfdffffffu);
    wr32(u + 0xc8, fl); wr32(u + 0xc8, fl | 0x4000000u);
    const uint32_t act = e.levelAct(e.roomLevel(room)) & 0xff;
    wr8(u + 0x18, (uint8_t)act);
    wr32(u + 0x1c, rd32(game + 0xbc + act * 4));
    wr32(u + 0x74, 0);
    e.initUnit(u, game + 0xd0);
    wr32(u + 0xc4, rd32(u + 0xc4) | 0x10);
    wr32(u + 0xd0, 0xb);
    uint32_t id = rd32(game + 0x98) + 1;
    if (id == 0xffffffffu) id = 1;
    wr32(game + 0x98, id);
    const uint32_t pd = e.alloc(pool, 0x38, 0x265);
    wr32(u + 0x14, pd);
    for (uint32_t i = 0; i < 14; i++) wr32(pd + 4 * i, 0);
    wr32(pd, e.objTxt(objId));
    wr32(u + 0x10, mode);
    e.setUnitState(u);
    e.place(u, id, room, x, y);
    e.attach(room, y, u, x, game, 1);
    const uint32_t mn = rd32(row + 4);
    int32_t sid = (int32_t)(e.roll(u + 0x20, rd32(row + 8) - mn) + mn);
    if (sid == 5 || sid == 4) sid = 2;
    wr8(rd32(u + 0x14) + 4, (uint8_t)sid);
    e.setShrine(u, e.shrineTxt((uint32_t)sid));
    return u;
}
