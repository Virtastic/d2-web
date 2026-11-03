// 1.13c D2Game+0x6A560 well refill (retail layout). EAX = objects.txt record, ESI = player, [esp+4] = game (ret 4); the first
// push of the prologue (ESI) is the argument of the max-life getter, which pops it. Record +0x17C = parm[1], +0x184 = flags.
// Life (flags & 2), mana (flags & 1) and stamina (no flag) are each raised by max * parm[1] >> 8 (32-bit unsigned multiply,
// capped at max) when below max: set the stat, then the attribute update (0xCC2F0, EDX = stat, ECX = player). Then the poison
// (2) and freeze (1) state lists are removed and freed, the npc-state removal (0xBC920, EBX = player) and the pet heal
// (0x96AF0: stack player and &result, EDI = game, EBX = callback 0x89DE0) run. Returns the local result: 1 once any step acted.
// The callees ride in Env so a probe records them.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2G6A560Env {
    uint32_t (*maxGet)(uint32_t pl, uint32_t which);                          // which 0 life 0xA3A0, 1 mana 0xA4EA, 2 stamina 0xA460
    uint32_t (*getStat)(uint32_t pl, uint32_t stat, uint32_t layer);          // 0xA42A
    void     (*setStat)(uint32_t pl, uint32_t stat, uint32_t val, uint32_t layer);   // 0xA472
    void     (*attr)(uint32_t pl, uint32_t stat, uint32_t val);               // 0xCC2F0
    uint32_t (*stateList)(uint32_t pl, uint32_t state);                       // 0xA5D4
    void     (*removeState)(uint32_t pl, uint32_t list);                      // 0xABB6
    void     (*freeList)(uint32_t list);                                      // 0xABAA
    uint32_t (*npcStates)(uint32_t pl);                                       // 0xBC920
    void     (*pets)(uint32_t pl, uint32_t* result, uint32_t game);           // 0x96AF0
};

static inline uint32_t rd32_6a560(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_6A560(uint32_t rec, uint32_t pl, uint32_t game, const D2G6A560Env* e)
{
    uint32_t result = 0, flags = 0;
    static const uint8_t kStat[3] = { 6, 8, 10 };
    for (uint32_t i = 0; i < 3; ++i) {
        const uint32_t mx = e->maxGet(pl, i);
        const uint32_t cur = e->getStat(pl, kStat[i], 0);
        if (i == 0) flags = rd32_6a560(rec + 0x184);
        const bool gate = i == 0 ? (flags & 2) != 0 : i == 1 ? (flags & 1) != 0 : true;
        if (cur < mx && gate) {
            uint32_t nv = (uint32_t)(rd32_6a560(rec + 0x17C) * mx) >> 8;
            nv += cur;
            if (mx < nv) nv = mx;
            e->setStat(pl, kStat[i], nv, 0);
            e->attr(pl, kStat[i], nv);
            result = 1;
        }
    }
    for (uint32_t st = 2; st >= 1; --st) {
        const uint32_t list = e->stateList(pl, st);
        if (list) { e->removeState(pl, list); e->freeList(list); result = 1; }
    }
    if (e->npcStates(pl)) result = 1;
    e->pets(pl, &result, game);
    return result;
}
