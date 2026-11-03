// 1.13c D2Game+0x6A720: wrapper of 0x6A330. Register convention: EAX = unit type, ECX = caster (may be 0), stack word 0 = game, word 1 = unit id, ret 8.
// u = 0xE03A0(type, game, id); none gives 0. With a caster: kind 1 and 0xA520(u[4])[0x16D] == 0 gives 1; else 0xAAA8(caster, u) == 0 gives 1.
// Then flag = 1, 0x6A330(&flag, game, id, caster, type) and the result is the flag.
#include <cstdint>
#include <cstring>
#include "d2game_6a330.h"

static inline uint32_t rd32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static inline uint32_t rd8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }

// 1.13c D2Game+0x6A720
uint32_t D2Game_6A720(uint32_t nType, uint32_t pCaster, uint32_t pGame, uint32_t nId, const D2GameSkillEnv* pEnv)
{
    const uint32_t u = pEnv->findUnit(nType & 0xFFFF, pGame, nId);
    if (!u) return 0;
    if (pCaster) {
        if (rd32(pCaster, 0) == 1 && rd8(pEnv->classRec(rd32(u, 4)), 0x16D) == 0) return 1;
        if (pEnv->aaa8(pCaster, u) == 0) return 1;
    }
    uint32_t flag = 1;
    pEnv->skill330(&flag, pGame, nId, pCaster, nType);
    return flag;
}
