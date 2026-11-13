// Skill-stat evaluator entry reached through a pointer table. ECX = skill id, EDX = selector word, stack: one unused word and a
// pointer to a holder (ret 8). No holder: 0. The unit is the holder's first dword. With a unit, the skill node is looked up (0x4E480,
// EAX = unit, ESI = skill id); the level is node+0x28, plus the bonus levels (0x4FCB0, EAX = node, ESI = unit) when node+0x34 is -1,
// floored at 0 and capped (signed) at the first word of the table behind module pointer +0xA0B50. A missing unit or node leaves level 0.
// The result is the evaluator 0x51070 (stdcall, four words) on (unit, selector word, skill id, level). The 0x6B430 row is the same
// code reading the unit from the holder's second dword.
#include <cstdint>
#include <cstring>

struct D2SkillEvalSeam {
    uint32_t (*pfnFindSkill)(uint32_t pUnit, int32_t nSkill);
    int32_t (*pfnBonusLevels)(uint32_t pNode, uint32_t pUnit);
    uint32_t (*pfnEval)(uint32_t pUnit, uint32_t nSel, uint32_t nSkill, uint32_t nLevel);
    const int32_t* pMaxLevel;
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
}

// 1.13c D2Common+0x51AF0
extern "C" uint32_t D2Common_51AF0_SkillEval(const D2SkillEvalSeam* s, uint32_t nSkill, uint32_t nSel, uint32_t pHolder)
{
    if (!pHolder) return 0;
    const uint32_t pUnit = R32(pHolder);
    int32_t nLevel = 0;
    if (pUnit) {
        const uint32_t pNode = s->pfnFindSkill(pUnit, (int32_t)nSkill);
        if (pNode) {
            int32_t n = (int32_t)R32(pNode + 0x28);
            if (R32(pNode + 0x34) == 0xffffffffu) n += s->pfnBonusLevels(pNode, pUnit);
            if (n < 0) n = 0;
            nLevel = n < *s->pMaxLevel ? n : *s->pMaxLevel;
        }
    }
    return s->pfnEval(pUnit, nSel, nSkill, (uint32_t)nLevel);
}
