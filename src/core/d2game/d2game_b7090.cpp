// 1.13c D2Game+0xB7090 -- AIBAAL_GetTarget: Baal's pick of a target among the game's target-node lists.
// Retail: stdcall (game, unit, pMax, pCount, pArgs, pfCull), ret 0x18; pfCull is fastcall (unit, target).
// Lists 0..7 (game+0x10F8): a list whose first unit (asserted a player) passes pfCull is scored whole -- the first node 0 when it
// is dead (mode 0x11) or dying (mode 0), every later node through the score callee 0xB6DA0 -- and every node counts.
// List 8: every node in the unit's act (byte +0x18) is scored and counted. List 9: the best node in the act is a fallback that
// replaces the pick when it lies within 5 of the unit's position, the pick is not in melee range, and a fresh path toward the pick fails.
// The callees come in as seams so the FN probe can record them. Raw 1.13c offsets.
#include <cstdint>
#include <cstring>

struct D2GB7090Seams {
    int32_t (*pathX)(uint32_t path);                                   // D2Common #10867 via 0xA454
    int32_t (*pathY)(uint32_t path);                                   // D2Common #10750 via 0xA3B8
    int32_t (*cull)(uint32_t fn, uint32_t unit, uint32_t target);      // the pfCull argument (fastcall)
    int32_t (*score)(uint32_t target, uint32_t args, uint32_t unit);   // 0xB6DA0: ECX target, EAX pArgs, stack unit
    int32_t (*dist)(uint32_t unit, int32_t x, int32_t y);              // 0xB08F0: EAX unit, stack x, y
    int32_t (*melee)(uint32_t unit, uint32_t target, int32_t z);       // D2Common #11138 via 0xA754
    void (*setTarget)(uint32_t path, uint32_t target);                 // #10336 via 0xA5FE
    void (*setType)(uint32_t path, int32_t v);                         // via 0xAAFC
    void (*setUnit)(uint32_t path, uint32_t unit, int32_t z);          // #10334 via 0xAAF0
    int32_t (*compute)(uint32_t path);                                 // #10800 via 0xAC70
    void (*halt)(uint32_t line);                                       // the assert (0x9E, 0xB3): no return
};

static inline uint32_t rd(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline uint8_t rb(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }

// 1.13c D2Game+0xB7090
extern "C" uint32_t d2game_b7090_port(uint32_t pGame, uint32_t pUnit, int32_t* pMax, int32_t* pCount, uint32_t pArgs,
                                      uint32_t pfCull, const D2GB7090Seams* s)
{
    if (!pfCull) { s->halt(0x9E); return 0; }
    const uint32_t lists = pGame + 0x10F8;
    uint32_t target = 0; int32_t nMax = 0, nCount = 0;
    const uint32_t type = rd(pUnit + 0);
    const bool stat = type == 2 || type == 4 || type == 5;
    const uint32_t path = rd(pUnit + 0x2C);
    const int32_t nX = stat ? (int32_t)rd(path + 0xC) : (path ? s->pathX(path) : 0);
    const int32_t nY = stat ? (int32_t)rd(path + 0x10) : (path ? s->pathY(path) : 0);
    const uint8_t act = rb(pUnit + 0x18);
    for (int i = 0; i < 8; ++i) {
        uint32_t node = rd(lists + 4 * i);
        if (!node) continue;
        uint32_t t = rd(node);
        if (!t || rd(t) != 0) { s->halt(0xB3); return 0; }
        if (!s->cull(pfCull, pUnit, t)) continue;
        const uint32_t mode = rd(t + 0x10);
        int32_t sc = (mode == 0x11 || mode == 0) ? 0 : s->score(t, pArgs, pUnit);
        for (;;) {
            if (nMax < sc) { nMax = sc; target = t; }
            node = rd(node + 8); ++nCount;
            if (!node) break;
            t = rd(node);
            sc = s->score(t, pArgs, pUnit);
        }
    }
    for (uint32_t node = rd(lists + 0x20); node; node = rd(node + 8)) {
        const uint32_t t = rd(node);
        if (act != rb(t + 0x18)) continue;
        const int32_t sc = s->score(t, pArgs, pUnit);
        if (nMax < sc) { nMax = sc; target = t; }
        ++nCount;
    }
    uint32_t pTemp = 0; int32_t nTemp = 0;
    for (uint32_t node = rd(lists + 0x24); node; node = rd(node + 8)) {
        const uint32_t t = rd(node);
        if (act != rb(t + 0x18)) continue;
        const int32_t sc = s->score(t, pArgs, pUnit);
        if (nTemp < sc) { nTemp = sc; pTemp = t; }
    }
    if (pTemp && s->dist(pTemp, nX, nY) < 5 && !s->melee(pUnit, target, 0)) {
        const uint32_t p = rd(pUnit + 0x2C);
        s->setTarget(p, target);
        s->setType(p, 2);
        s->setUnit(p, pUnit, 0);
        if (!s->compute(p)) { target = pTemp; nMax = nTemp; }
    }
    *pCount = nCount;
    *pMax = nMax;
    return target;
}
