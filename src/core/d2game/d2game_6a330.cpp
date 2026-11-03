// 1.13c D2Game+0x6A330: run the skill-spawn handler of the unit with id nId. Register convention: EAX = out flag (set to 1), EBX = game, EDX = id, EDI = caster
// (may be 0), stack word 0 = unit type (low word is the lookup type), ret 4.
// u = 0xE03A0(type, game, id); cls = u ? u[4] : -1; rec = 0xB156(u). A caster of kind 0 is refused (result 0) when caster[0x6C] is set, or its player data
// (0xA4C0) has +0x4C set, or 0xA6A6(caster[0x60]) holds and cls != 0x10B. Then idx = rec[0x1B3]: idx >= 0x65 is Fog fatal line 0xCF8; with cls not 0x79/0x7A/0x16
// and a table[idx] handler, 0xA478(u) runs and the handler is called with ECX = {game, u, caster, game[0x10F0], cls}; otherwise 0.
#include <cstdint>
#include <cstring>
#include "d2game_6a330.h"

static inline uint32_t rd32(uint32_t p, uint32_t o) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)(p + o), 4); return v; }
static inline uint32_t rd8(uint32_t p, uint32_t o) { return *(const uint8_t*)(uintptr_t)(p + o); }

// 1.13c D2Game+0x6A330
uint32_t D2Game_6A330(uint32_t* pFlag, uint32_t pGame, uint32_t nId, uint32_t pCaster, uint32_t nType, const uint32_t* pTable, const D2GameSkillEnv* pEnv)
{
    *pFlag = 1;
    const uint32_t u = pEnv->findUnit(nType & 0xFFFF, pGame, nId);
    const uint32_t cls = u ? rd32(u, 4) : 0xFFFFFFFFu;
    const uint32_t rec = pEnv->unitRec(u);
    if (pCaster && rd32(pCaster, 0) == 0) {
        if (rd8(pCaster, 0x6C)) return 0;
        if (rd32(pEnv->playerData(pCaster), 0x4C)) return 0;
        if (pEnv->a6a6(rd32(pCaster, 0x60)) && cls != 0x10B) return 0;
    }
    const uint32_t idx = rd8(rec, 0x1B3);
    if (idx >= 0x65) { pEnv->fatal(0xCF8); return 0; }
    if (cls != 0x79 && cls != 0x7A && cls != 0x16 && pTable[idx]) {
        pEnv->a478(u);
        const uint32_t blk[5] = { pGame, u, pCaster, rd32(pGame, 0x10F0), cls };
        return pEnv->callHandler(pTable[idx], blk);
    }
    return 0;
}
