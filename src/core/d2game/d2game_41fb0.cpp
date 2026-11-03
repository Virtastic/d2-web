// 1.13c D2Game+0x41FB0 -- drains eight stat pools into a durability-like stat.
// Retail: stdcall, five stack words (ret 0x14): unit, attacker, pools, record index, extra (the fourth argument of the
// stat getter 0xAC9A). Every callee is a seam so the FN probe can log it exactly as the capture's hooks do.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

struct D2Game41FB0Env {
    const uint8_t* dataTables;                                            // *sgptDataTables
    uint32_t (*stateList)(uint32_t unit, uint32_t state);                 // 0xA5D4 (Ordinal 10871)
    int32_t  (*stat)(uint32_t unit, uint32_t id, uint32_t a, uint32_t b); // 0xAC9A (Ordinal 10786)
    int32_t  (*curStat)(uint32_t unit, uint32_t id, uint32_t layer);      // 0xA42A (Ordinal 10973)
    int32_t  (*isPlayer)(uint32_t unit);                                  // 0xA7F6 (Ordinal 11104)
    int32_t  (*scale)(int32_t a, int32_t c, int32_t d);                   // 0x214D0 (EAX, ECX, EDX)
    void     (*setStat)(uint32_t unit, uint32_t id, int32_t v, uint32_t layer); // 0xA472 (Ordinal 10887)
    uint32_t (*posX)(uint32_t unit);                                      // 0x1100 (ECX)
    uint32_t (*posY)(uint32_t unit);                                      // 0x1130 (ECX)
    uint32_t (*dirAt)(uint32_t unit, uint32_t x, uint32_t y);             // 0xA7A8
    uint32_t (*dirMap)(uint32_t v);                                       // 0xAD1E
    void     (*setState)(uint32_t unit, int32_t v, uint32_t z);           // 0xA83E
    void     (*clearState)(uint32_t unit, int32_t state, uint32_t z);     // 0xA3CA (Ordinal 10945)
    void     (*attach)(uint32_t unit, uint32_t list);                     // 0xABB6 (Ordinal 11111)
    void     (*freeList)(uint32_t list);                                  // 0xABAA (Ordinal 11108)
};

static inline int32_t  r32(uint32_t a) { int32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
static inline int32_t  r16(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
static inline void     w32(uint32_t a, int32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
static inline int32_t  dt32(const D2Game41FB0Env* e, uint32_t o) { int32_t v; std::memcpy(&v, e->dataTables + o, 4); return v; }

// 1.13c D2Game+0x41FB0
extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_41FB0(uint32_t unit, uint32_t attacker, uint32_t pools, int32_t recIdx, uint32_t extra, const D2Game41FB0Env* e)
{
    static const int32_t kPool[8][2] = { {0x08, 0}, {0x10, 0}, {0x1c, 0}, {0x24, 0}, {0x20, 0}, {0x38, 1}, {0x3c, 1}, {0x40, 1} };   // 0x6FD1C3E8
    if (!unit || !pools) return 0;
    if (recIdx < 0 || recIdx >= dt32(e, 0xba0)) return 0;
    const uint32_t rec = (uint32_t)dt32(e, 0xb98) + (uint32_t)recIdx * 0x23c;
    if (!rec) return 0;
    const int32_t state = r16(rec + 0x80);
    if (state < 0 || state >= dt32(e, 0xc4)) return 0;
    if (!e->stateList(unit, (uint32_t)state)) return 0;
    const int32_t a = e->stat(unit, (uint32_t)r32(rec + 0x138), (uint32_t)recIdx, extra);
    if (a <= 0) return 0;
    int32_t cur = e->curStat(unit, 8, 0);
    int32_t n = e->stat(unit, (uint32_t)r32(rec + 0x13c), (uint32_t)recIdx, extra);
    if (n <= 0) n = 1;
    bool all = true;
    if (attacker && (r32(attacker) != 1 || e->isPlayer(attacker) != 0)) all = false;
    int32_t total = 0;
    for (int i = 0; i < 8; ++i) {
        if (kPool[i][1] != 0 && !all) continue;
        const uint32_t pp = pools + (uint32_t)kPool[i][0];
        const int32_t pool = r32(pp);
        if (pool <= 0) continue;
        int32_t take = e->scale(a, 100, pool);
        const int32_t cap = e->scale(0x10, n, cur);
        if (!(take < cap)) take = cap;
        total += take;
        w32(pp, pool - take);
        cur -= e->scale(n, 0x10, take);
        if (cur <= 0) cur = 0;
    }
    if (cur <= 0) cur = 0;
    e->setStat(unit, 8, cur, 0);
    if (attacker && total > 0) {
        const int32_t s = r16(rec + 0x10c);
        if (s >= 0 && s <= dt32(e, 0xbc0)) {
            const uint32_t y = e->posY(attacker);
            const uint32_t x = e->posX(attacker);
            const uint32_t d = e->dirAt(unit, x, y) & 0xff;
            const uint32_t m = e->dirMap(d) & 0xff;
            e->setState(unit, s + (int32_t)m, 0);
        }
    }
    if (cur > 0) return 1;
    const uint32_t sl = e->stateList(unit, (uint32_t)(int32_t)r16(rec + 0x80));
    if (sl) { e->attach(unit, sl); e->freeList(sl); return 1; }
    e->clearState(unit, r16(rec + 0x80), 0);
    return 1;
}
