// src/core/d2game/d2game_41c30.cpp -- 1.13c D2Game.dll 0x41C30, written from the retail disassembly.
// MISSMODE_SrvDo34_BaalTauntControl: ecx = game, edx = missile, plain ret. Callees are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }
static inline void     wr32(void* p, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)p + o, &v, 4); }

struct D2Game41C30Env {
    const uint8_t* dataTables;                                                  // *sgptDataTables
    int32_t  (*stat)(uint32_t unit);                                            // 0xB28E (Ordinal 10985)
    uint32_t (*a454)(uint32_t sub);                                             // 0xA454 (Ordinal 10867)
    void     (*seed)(uint32_t seedPtr, uint32_t v);                             // 0xA628 (ecx seed, edx value)
    uint32_t (*owner)(uint32_t game, uint32_t type, uint32_t guid);             // 0xE03A0 (eax type, ecx game, edx guid)
    int32_t  (*skill)(uint32_t unit);                                           // 0xB26A (Ordinal 10240)
    int32_t  (*level)(uint32_t unit);                                           // 0xB0FC (Ordinal 11029)
    uint32_t (*create)(uint32_t game, const uint32_t* request);                 // 0x6F930 (ecx game, edx request)
    void     (*after)(uint32_t game, uint32_t zero, uint32_t one);              // 0x3EDE0 (stdcall game, 0, 1)
    uint32_t (*tail)(uint32_t game, uint32_t unit);                             // 0x41AE0 (ecx game, edx unit)
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_41C30(uint32_t game, uint32_t unit, const D2Game41C30Env* e) {   // 1.13c D2Game+0x41C30
    const uint8_t* dt = e->dataTables;
    int32_t cls = unit ? (int32_t)rd32((const void*)(uintptr_t)unit, 4) : -1;
    if (!unit || cls < 0 || cls >= (int32_t)rd32(dt, 0xB6C)) return 2;
    const uint8_t* row = (const uint8_t*)(uintptr_t)(rd32(dt, 0xB64) + (uint32_t)cls * 0x1A4);
    uint8_t* u = (uint8_t*)(uintptr_t)unit;
    const int32_t st = e->stat(unit);
    uint32_t n = 0;
    while (n < 3 && rd16s(row, 0x18 + 2 * n) >= 0 && (int32_t)rd32(row, 0x3C + 4 * n) > 0) n++;
    if (!n) return 2;
    if (st >= (int32_t)rd32(row, 0x38)) {
        const int32_t type = (int32_t)rd32(u, 0);
        const uint32_t sub = rd32(u, 0x2C);
        uint32_t v;
        if (type == 2 || (type > 3 && type <= 5)) v = rd32((const void*)(uintptr_t)sub, 0xC);
        else v = sub ? e->a454(sub) : 0;
        e->seed(unit + 0x20, v);
        const uint64_t p = (uint64_t)rd32(u, 0x20) * 0x6AC690C5ull + rd32(u, 0x24);
        wr32(u, 0x24, (uint32_t)(p >> 32)); wr32(u, 0x20, (uint32_t)p);
        const uint32_t lo = (uint32_t)p;
        const uint32_t idx = (n & (n - 1)) ? lo % n : ((n - 1) & lo);
        const int32_t div = (int32_t)rd32(row, 0x3C + 4 * idx);
        if (st % div == 0) {
            uint32_t req[0x17] = {};
            if ((rd32(u, 0xC8) >> 10) & 1) req[1] = e->owner(game, rd32(u, 0x94), rd32(u, 0x98));
            req[2] = unit;
            req[11] = (uint32_t)e->skill(unit);
            req[12] = (uint32_t)e->level(unit);
            req[4] = (uint32_t)(int32_t)rd16s(row, 0x18 + 2 * idx);
            if (e->create(game, req)) e->after(game, 0, 1);
        }
    }
    return e->tail(game, unit);
}
