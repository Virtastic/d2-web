// 1.13c aura callbacks that 1.10f shares with sub_6FCF5DE0 (SkillAss.cpp); retail keeps a second copy, split out here.
#include <test_seam.h>
#include "SKILLS/Skills.h"

#include <cstring>

// 1.13c D2Game+0xA1210 (same body as sub_6FCF5DE0 / D2Game+0x92CC0: the damage record in pArgs is copied and dispatched to sub_6FD10140)
int32_t __fastcall SKILLS_DamageAuraCallback_A1210(D2AuraCallbackStrc* pAuraCallback, D2UnitStrc* pUnit)
{
    D2DamageStrc damage = {};

    memcpy(&damage, (D2DamageStrc*)pAuraCallback->pArgs, sizeof(damage));
    D2_CALL(sub_6FD10140, pAuraCallback->pGame, pAuraCallback->pOwner, pUnit, &damage, 0);

    return 1;
}
