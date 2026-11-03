// MONSTERUNIQUE_StealBeltItem (1.13c D2Game+0x24440): ECX = game, EDX = unit, stack = nUMod (unused), bUnique (ret 8). Retail-layout (32-bit offsets) rule; every engine callee is an Env member.
// unit+0x20/+0x24 = the unit seed (lo, hi), unit+0x2C = path, target+0x60 = inventory.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G24440Env {
    void (*e07a0)(uint32_t unit);                                              // 0xE07A0 (EAX unit), result unused
    uint32_t (*pathTarget)(uint32_t path);                                     // thunk 0xA598 (#10392)
    uint32_t (*beltSlot)(uint32_t inventory, uint32_t slot);                   // thunk 0xA8E6 (#10455)
    uint32_t (*cursorItem)(uint32_t inventory);                                // thunk 0xA6A6 (#11017)
    uint32_t (*duplicate)(uint32_t game, uint32_t item, uint32_t one);         // 0x107E0 (EAX game, stack item, 1)
    void (*unitCoords)(uint32_t unit, uint32_t* xy);                           // thunk 0xA544 (#10141)
    uint32_t (*room)(uint32_t unit);                                           // thunk 0xA478 (#10331)
    uint32_t (*freeSpace)(const uint32_t* coord, uint32_t room, uint32_t* ret, uint32_t one);   // 0xE930 (ESI coord, stack room, &ret, 1)
    void (*setItemFlag)(uint32_t item, uint32_t flag, uint32_t on);            // thunk 0xA65E
    void (*dropItem)(uint32_t game, uint32_t target, uint32_t item, uint32_t room, uint32_t x, uint32_t y);   // 0xE990 (EAX y, ECX item, EBX room, stack game, target, x)
    uint32_t (*sub1100)(uint32_t item);                                       // 0x1100 (ECX item)
    void (*sub0D3950)(uint32_t item, uint32_t game);                           // 0xD3950 (EAX item, stack game)
    void (*sub0D2540)(uint32_t v, uint32_t target);                            // 0xD2540 (EAX v, stack target)
    void (*fatal)();                                                           // the free-space assert: _exit(-1)
};

// 1.13c D2Game+0x24440
void MONSTERUNIQUE_StealBeltItem(uint32_t game, uint32_t unit, uint32_t /*nUMod*/, uint32_t bUnique, const D2G24440Env& env)
{
    if (!bUnique) return;
    const uint64_t v = (uint64_t)rd32(unit + 0x20) * 0x6AC690C5u + rd32(unit + 0x24);
    wr32(unit + 0x20, (uint32_t)v); wr32(unit + 0x24, (uint32_t)(v >> 32));
    if ((uint32_t)v % 100 < 30) return;
    env.e07a0(unit);
    const uint32_t target = env.pathTarget(rd32(unit + 0x2C));
    if (target == unit || !target || rd32(target) != 0) return;
    const uint32_t inv = rd32(target + 0x60);
    uint32_t item = 0;
    for (uint32_t i = 0; i < 16; i++) { item = env.beltSlot(inv, i); if (item) break; }
    if (!item) return;
    if (rd32(item) != 4 || rd32(item + 0x10) != 2 || env.cursorItem(inv)) return;
    const uint32_t dup = env.duplicate(game, item, 1);
    uint32_t coord[2], ret[2];
    env.unitCoords(target, coord);
    const uint32_t room = env.freeSpace(coord, env.room(target), ret, 1);
    if (!room) { env.fatal(); return; }
    env.setItemFlag(dup, 0x2000, 1);
    env.dropItem(game, target, dup, room, ret[0], ret[1]);
    const uint32_t r = env.sub1100(item);
    env.sub0D3950(item, game);
    env.sub0D2540(r, target);
}
