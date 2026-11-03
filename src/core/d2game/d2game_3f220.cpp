// The exploding/freezing-arrow and meteor-centre missile hit on retail's 32-bit layout (the vendored typed form is
// MISSMODE_SrvHit04_ExplodingArrow_FreezingArrow_RoyalStrikeMeteorCenter in d2moo/src/D2Game/MISSILES/MissMode.cpp).
// ECX = game, EDX = missile, one stack word (the hit target, ret 4). A missile with no Missiles.txt row answers 1.
// Otherwise a zeroed 23-dword create request gets the owner (the server unit the missile's +0x94/+0x98 type/guid
// names, when flag bit 10 of +0xC8 is set), the missile itself, its level and skill; each of the row's four positive
// sub-missile ids (+0x24) is created in turn, and when one is created and the row's +0x4C is positive the hit target's
// trigger hooks run (game, target, 1, EDI = the created sub-missile). Answers 3.
#include "d2game_2e790.h"
using namespace d2g32;

struct D2G3F220Env {
    uint32_t dataTables;                                                    // sgptDataTables
    uint32_t (*serverUnit)(uint32_t game, uint32_t type, uint32_t guid);   // SUNIT_GetServerUnit (EAX type, ECX game, EDX guid)
    uint32_t (*level)(uint32_t missile);                                   // D2Common missile level
    uint32_t (*skill)(uint32_t missile);                                   // D2Common missile skill
    uint32_t (*create)(uint32_t game, uint32_t* req);                      // the missile creator (ECX game, EDX request)
    void     (*trigger)(uint32_t game, uint32_t sub, uint32_t target, uint32_t one);   // EDI = the created sub-missile
};

uint32_t MISSMODE_SrvHit04_32(uint32_t game, uint32_t missile, uint32_t target, const D2G3F220Env& env)
{
    if (!missile) return 1;
    const int32_t id = (int32_t)rd32(missile + 4);
    const uint32_t dt = env.dataTables;
    if (id < 0 || id >= (int32_t)rd32(dt + 0xB6C)) return 1;
    const uint32_t row = (uint32_t)id * 0x1A4 + rd32(dt + 0xB64);
    if (!row) return 1;
    uint32_t req[23] = {};
    if (rd32(missile + 0xC8) >> 10 & 1) req[1] = env.serverUnit(game, rd32(missile + 0x94), rd32(missile + 0x98));
    req[2] = missile;
    req[11] = env.level(missile);
    req[12] = env.skill(missile);
    for (uint32_t i = 0; i < 4; i++) {
        const int16_t sub = (int16_t)rd16(row + 0x24 + 2 * i);
        if (sub <= 0) continue;
        req[4] = (uint32_t)(int32_t)sub;
        const uint32_t created = env.create(game, req);
        if (created && (int32_t)rd32(row + 0x4C) > 0) { env.trigger(game, created, target, 1); }
    }
    return 3;
}
