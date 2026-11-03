// src/core/d2game/d2game_41520.cpp -- 1.13c D2Game.dll 0x41520, written from the retail disassembly.
// MISSMODE_SrvDo09_BatLightningBolt: ecx = game, edx = missile, plain ret. Callees are seams.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

static inline uint32_t rd32(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }
static inline int16_t  rd16s(const void* p, uint32_t o) { int16_t v; std::memcpy(&v, (const uint8_t*)p + o, 2); return v; }

struct D2Game41520Env {
    const uint8_t* dataTables;                                                  // *sgptDataTables
    uint32_t (*a454)(uint32_t sub);                                             // 0xA454 (Ordinal 10867)
    uint32_t (*a3b8)(uint32_t sub);                                             // 0xA3B8 (Ordinal 10750)
    uint32_t (*acc4)(uint32_t sub);                                             // 0xACC4 (Ordinal 10203)
    uint32_t (*owner)(uint32_t game, uint32_t type, uint32_t guid);             // 0xE03A0 (eax type, ecx game, edx guid)
    int32_t  (*skill)(uint32_t unit);                                           // 0xB26A (Ordinal 10240)
    int32_t  (*level)(uint32_t unit);                                           // 0xB0FC (Ordinal 11029)
    uint32_t (*create)(uint32_t game, const uint32_t* request);                 // 0x6F930 (ecx game, edx request)
    uint32_t (*tail)(uint32_t game);                                            // 0x3F320 (stdcall game)
};

extern "C" EMSCRIPTEN_KEEPALIVE uint32_t D2Game_41520(uint32_t game, uint32_t unit, const D2Game41520Env* e) {   // 1.13c D2Game+0x41520
    const uint8_t* dt = e->dataTables;
    // retail reads address 0x18 (a crash) when the unit or its class row is invalid; only the valid path is reachable
    int32_t cls = unit ? (int32_t)rd32((const void*)(uintptr_t)unit, 4) : -1;
    if (!unit || cls < 0 || cls >= (int32_t)rd32(dt, 0xB6C)) return 2;
    const uint8_t* row = (const uint8_t*)(uintptr_t)(rd32(dt, 0xB64) + (uint32_t)cls * 0x1A4);
    const uint8_t* u = (const uint8_t*)(uintptr_t)unit;
    const int32_t type = (int32_t)rd32(u, 0);
    const uint32_t sub = rd32(u, 0x2C);
    const bool direct = type == 2 || (type > 3 && type <= 5);
    uint32_t a = 0, b = 0;
    if (direct) { a = rd32((const void*)(uintptr_t)sub, 0xC); b = rd32((const void*)(uintptr_t)sub, 0x10); }
    else if (sub) { a = e->a454(sub); b = e->a3b8(sub); }
    if (e->acc4(sub)) {
        uint32_t req[0x17] = {};
        req[0] = 1;
        if ((rd32(u, 0xC8) >> 10) & 1) req[1] = e->owner(game, rd32(u, 0x94), rd32(u, 0x98));
        req[5] = a; req[6] = b;
        req[11] = (uint32_t)e->skill(unit);
        req[12] = (uint32_t)e->level(unit);
        req[4] = (uint32_t)(int32_t)rd16s(row, 0x18);
        e->create(game, req);
    }
    return e->tail(game);
}
