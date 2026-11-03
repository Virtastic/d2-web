// 1.13c D2Game+0x5C100 ACT5Q5_UnitIterate_SetPrimaryGoalDoneForPartyMembers: ECX = game, EDX = unit, one stack
// word (pData, unused; ret 4). Needs a room (#10331 = 0xA478) and the quest record with id 0x23 in the game's list (game+0x10F4, head, then
// next at +0xF4; none = fatal 0x2D5). The unit must be at least 20 * (difficulty + 1) in level (#10587 stat 0xC), in level 0x78 (0xA556)
// and have quest (record+0xE0) state 0 clear; then states 0xD and 0 are set, ACT5Q5_RewardPlayer (0x5BF70: EAX = unit, stack game) runs, and
// when the unit has a party (0xDFA60: EAX = unit, AX) its members are iterated with callback 0x5C080 (0x9C160: EAX = party, ESI = game,
// EBX = 0, stack callback). Returns 0. A missing quest record is the fatal (the probe traps). Callees ride in Env.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G5C100Env {
    uint32_t (*getRoom)(uint32_t unit);                                               // #10331 (0xA478)
    int32_t  (*baseStat)(uint32_t unit, uint32_t stat, uint32_t layer);               // #10587 (0xA3E2)
    uint32_t (*levelId)(uint32_t room);                                                // 0xA556
    uint32_t (*playerData)(uint32_t unit);                                             // #10920 (0xA4C0)
    uint32_t (*questState)(uint32_t flags, uint32_t quest, uint32_t state);            // #10174 (0xA568)
    void     (*setState)(uint32_t flags, uint32_t quest, uint32_t state);              // #10003 (0xA57A)
    void     (*reward)(uint32_t unit, uint32_t game);                                  // 0x5BF70
    uint16_t (*partyId)(uint32_t unit);                                                // 0xDFA60
    void     (*iterate)(uint32_t party, uint32_t game);                                // 0x9C160 with callback 0x5C080
    void     (*fatal)(uint32_t line);
};

static inline uint32_t rd32_5c100(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_5C100(uint32_t game, uint32_t unit, uint32_t, const D2G5C100Env* e)
{
    const uint32_t room = e->getRoom(unit);
    if (!room) return 0;
    uint32_t node = 0;
    const uint32_t list = rd32_5c100(game + 0x10F4);
    if (list) {
        node = rd32_5c100(list);
        while (node && rd32_5c100(node) != 0x23) node = rd32_5c100(node + 0xF4);
    }
    if (!node) e->fatal(0x2D5);
    const uint8_t diff = *(const uint8_t*)(uintptr_t)(game + 0x6D);
    if (e->baseStat(unit, 0xC, 0) < (int32_t)(((uint32_t)diff + 1) * 20)) return 0;
    if (e->levelId(room) != 0x78) return 0;
    const uint32_t flags = rd32_5c100(e->playerData(unit) + 0x10 + diff * 4);
    const uint32_t quest = rd32_5c100(node + 0xE0);
    if (e->questState(flags, quest, 0) != 0) return 0;
    e->setState(flags, rd32_5c100(node + 0xE0), 0xD);
    e->setState(flags, rd32_5c100(node + 0xE0), 0);
    e->reward(unit, game);
    const uint16_t party = e->partyId(unit);
    if (party == 0xFFFF) return 0;
    e->iterate(party, game);
    return 0;
}
