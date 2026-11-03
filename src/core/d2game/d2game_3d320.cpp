// 1.13c D2Game+0x3D320: MISSMODE_CreatePlagueJavelin_PoisonJavelin_PoisonTrapHitSubmissiles
// written from the 1.13c decompile and
// disassembly as a rule over seams. Register convention: ebx = nRange, esi = pOrigin; stack: pGame, nMissileId, nLoops (ret 0xC).
// Returns the created missile (0 on any refusal).
#include <cstdint>
#include <cstring>

struct D2Game3D320Env {
    int32_t (*pathOk)(void* path);                                 // D2Common #10203 (0xACC4), arg = origin+0x2C
    uint32_t (*getUnit)(uint32_t game, uint32_t type, uint32_t guid); // 0xE03A0 (eax = type, ecx = game, edx = guid)
    int32_t (*posX)(void* rec);                                    // #10867 (0xA454)
    int32_t (*posY)(void* rec);                                    // #10750 (0xA3B8)
    int32_t (*skill)(void* unit);                                  // #10240 (0xB26A)
    int32_t (*level)(void* unit);                                  // #11029 (0xB0FC)
    uint32_t (*create)(uint32_t game, uint32_t* req);              // 0x6F930, edx = the 23-dword request
};

static inline uint32_t d3_rd(const void* p, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)p + o, 4); return v; }

uint32_t D2Game_3D320(int32_t range, void* origin, uint32_t game, uint32_t missileId, int32_t loops, const D2Game3D320Env* e)
{
    void* rec = (void*)(uintptr_t)d3_rd(origin, 0x2C);
    if (!e->pathOk(rec)) return 0;
    uint32_t req[23] = {};
    req[0] = 5;
    if (range > 0) { req[19] = (uint32_t)range; req[0] = 0x8005; }
    if (loops > 0) { req[13] = (uint32_t)loops; req[0] |= 8; }
    if (!((d3_rd(origin, 0xC8) >> 10) & 1)) return 0;
    const uint32_t owner = e->getUnit(game, d3_rd(origin, 0x94), d3_rd(origin, 0x98));
    req[1] = owner;
    if (!owner) return 0;
    req[2] = (uint32_t)(uintptr_t)origin;
    req[4] = missileId;
    const uint32_t type = d3_rd(origin, 0);
    const bool pet = type == 2 || (type > 3 && type < 6);
    req[5] = pet ? d3_rd(rec, 0xC) : rec ? (uint32_t)e->posX(rec) : 0;
    req[6] = pet ? d3_rd(rec, 0x10) : rec ? (uint32_t)e->posY(rec) : 0;
    const int32_t skill = e->skill(origin);
    const int32_t level = e->level(origin);
    req[0xB] = (uint32_t)skill;
    req[0xC] = (uint32_t)level;
    req[13] = (uint32_t)(2 * (level - 1));
    return e->create(game, req);
}
