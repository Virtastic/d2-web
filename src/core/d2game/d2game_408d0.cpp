// src/core/d2game/d2game_408d0.cpp -- 1.13c D2Game.dll 0x408D0, written from the retail disassembly.
// MISSMODE_SrvDo18_TowerChestSpawner: ecx = game, edx = missile, plain ret. Callees are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2Game408D0Env {
    const uint8_t* dataTables;                                                       // *sgptDataTables
    int32_t  (*frame)(uint32_t missile);                                             // 0xB102 (MISSILE_GetCurrentFrame)
    uint32_t (*targetX)(uint32_t missile);                                           // 0xB24C (object guid)
    void     (*sound)(uint32_t object, uint32_t id, uint32_t zero);                  // 0xDFFD0 (esi = object, stack id, 0)
    void     (*opFn)(const uint32_t* op);                                            // 0x6DBF0 (eax = 5-dword operate block)
    void     (*setTargetY)(uint32_t missile, uint32_t v);                            // 0xAF5E
    int32_t  (*targetY)(uint32_t missile);                                           // 0xB288
    void     (*coords)(uint32_t missile, uint32_t* xy);                              // 0xA544 (writes x, y)
    uint32_t (*roll)(uint32_t seedPtr, uint32_t limit);                              // 0x11D0 (ecx seed, esi limit)
    uint32_t (*room)(uint32_t missile);                                              // 0xA478
    uint32_t (*spot)(uint32_t room, const uint32_t* in, uint32_t* out, uint32_t one);// 0xE930 (esi = in)
    uint32_t (*levelId)(uint32_t room);                                              // 0xA556
    uint32_t (*monLevel)(uint32_t levelId, uint32_t difficulty, uint32_t expansion); // 0xADA8
    uint32_t (*itemId)(uint32_t code);                                               // 0xAE74
    uint32_t (*create)(uint32_t game, const uint32_t* drop, uint32_t zero);          // 0x11490 (33-dword block)
    uint32_t (*tail)(uint32_t game);                                                 // 0x3F320
};

// the object hash: game+0x1520[guid & 0x7F], chained through +0xE4, keyed on +0xC
static uint32_t find408d0(const uint8_t* g, uint32_t guid) {
    for (uint32_t n = rd32(g, 0x1520 + 4 * (guid & 0x7F)); n; n = rd32((const void*)(uintptr_t)n, 0xE4))
        if (rd32((const void*)(uintptr_t)n, 0xC) == guid) return n;
    return 0;
}

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_408D0(uint32_t game, uint32_t missile, const D2Game408D0Env* e) {   // 1.13c D2Game+0x408D0
    const uint8_t* dt = e->dataTables;
    int32_t cls = missile ? (int32_t)rd32((const void*)(uintptr_t)missile, 4) : -1;
    if (!missile || cls < 0 || cls >= (int32_t)rd32(dt, 0xB6C)) return 2;
    const uint8_t* row = (const uint8_t*)(uintptr_t)(rd32(dt, 0xB64) + (uint32_t)cls * 0x1A4);
    const uint8_t* g = (const uint8_t*)(uintptr_t)game;   // retail takes its fatal-error path when game is null; not reachable here
    const int32_t frame = e->frame(missile);
    if (frame == 1) {
        if (uint32_t o = find408d0(g, e->targetX(missile))) e->sound(o, 0x5C, 0);
    }
    if (frame == (int32_t)rd16s(row, 0x96) - (int32_t)rd32(row, 0x38)) {
        if (uint32_t o = find408d0(g, e->targetX(missile))) {
            const uint32_t op[5] = { game, o, 0, o + 0x20, rd32((const void*)(uintptr_t)o, 4) };
            e->opFn(op);
        }
        e->setTargetY(missile, 1);
    }
    if (e->targetY(missile)) {
        int32_t per = (int32_t)(rd32(row, 0x3C) << 2);
        if (per < 1) per = 1;
        if (frame % per == 0) {
            uint32_t xy[2];
            e->coords(missile, xy);
            const uint32_t range = rd32(row, 0x40), limit = 2 * range + 1;
            xy[0] += e->roll(missile + 0x20, limit) - range;
            xy[1] += e->roll(missile + 0x20, limit) - range;
            uint32_t out[2];
            const uint32_t room = e->spot(e->room(missile), xy, out, 1);
            if (room) {
                const uint32_t lvl = e->monLevel(e->levelId(room), g[0x6D], rd32(g, 0x70));
                uint32_t d[33] = {};
                d[0] = missile; d[2] = game;
                d[3] = lvl; d[5] = e->itemId(0x20646C67); d[6] = 3;
                d[7] = out[0]; d[8] = out[1]; d[9] = room;
                d[10] = 1;      // word at +0x28: wUnitInitFlags; the word at +0x2A is wItemFormat
                std::memcpy((uint8_t*)d + 0x2A, g + 0x78, 2);
                d[12] = 2;
                e->create(game, d, 0);
            }
        }
    }
    return e->tail(game);
}
