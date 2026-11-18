#include "MISSILES/MissMode.h"

#include <test_seam.h>

#include <algorithm>

#include <D2BitManip.h>
#include <D2Math.h>

#include <D2PacketDef.h>

#include <D2StatList.h>
#include <D2Dungeon.h>
#include <Units/Missile.h>
#include <DataTbls/MissilesIds.h>
#include <DataTbls/MissilesTbls.h>
#include <DataTbls/SkillsIds.h>
#include <DataTbls/SkillsTbls.h>
#include <DataTbls/ObjectsIds.h>
#include <DataTbls/ObjectsTbls.h>
#include <D2States.h>
#include <D2StatList.h>
#include <D2DataTbls.h>
#include <D2Skills.h>
#include <D2Combat.h>
#include <Path/Path.h>
#include <Path/PathMisc.h>
#include <Path/Step.h>
#include <D2Monsters.h>
#include <D2Collision.h>
#include <UselessOrdinals.h>


#include "AI/AiGeneral.h"
#include "AI/AiUtil.h"
#include "GAME/Clients.h"
#include "GAME/Event.h"
#include "GAME/SCmd.h"
#include "GAME/Targets.h"
#include "ITEMS/ItemMode.h"
#include "ITEMS/Items.h"
#include "MISSILES/Missiles.h"
#include "MONSTER/MonsterMode.h"
#include "MONSTER/MonsterRegion.h"
#include "MONSTER/MonsterSpawn.h"
#include "MONSTER/MonsterUnique.h"
#include "OBJECTS/ObjMode.h"
#include "QUESTS/Quests.h"
#include "QUESTS/ACT5/A5Q6.h"
#include "SKILLS/SkillAss.h"
#include "SKILLS/SkillBar.h"
#include "SKILLS/SkillDruid.h"
#include "SKILLS/SkillNec.h"
#include "SKILLS/SkillPal.h"
#include "SKILLS/Skills.h"
#include "UNIT/SUnit.h"
#include "UNIT/SUnitDmg.h"
#include "UNIT/SUnitEvent.h"


#pragma pack(push, 1)
struct D2HitFunc22ParamStrc
{
    D2UnitStrc* pOwner;
    D2UnitStrc* pOrigin;
    int32_t nSkillId;
    int32_t nLevel;
    int32_t nParam;
    int32_t nHitSubMissile;
};
#pragma pack(pop)


D2MissileUnitFindTableStrc stru_6FD2E5F8[] =
{
    /*[0]*/{ nullptr                                                  , COLLIDE_NONE                                       },
    /*[1]*/{ MISSMODE_UnitFindCallback_CanCollideWithGoodAlignmentUnit, COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER                   },
    /*[2]*/{ MISSMODE_UnitFindCallback_CanCollideWithMonster          , COLLIDE_MONSTER | COLLIDE_MISSILE_BARRIER                  },
    /*[3]*/{ MISSMODE_UnitFindCallback_CanCollideWithPlayerOrMonster  , COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER },
    /*[4]*/{ nullptr                                                  , COLLIDE_NONE                                       },
    /*[5]*/{ MISSMODE_UnitFindCallback_CanCollideWithMonster          , COLLIDE_MONSTER | COLLIDE_MISSILE_BARRIER                  },
    /*[6]*/{ nullptr                                                  , COLLIDE_MISSILE_BARRIER                                    },
    /*[7]*/{ MISSMODE_UnitFindCallback_CanMissileDestroy              , COLLIDE_MISSILE                                    },
    /*[8]*/{ MISSMODE_UnitFindCallback_CanCollideWithPlayerOrMonster  , COLLIDE_MONSTER | COLLIDE_PLAYER | COLLIDE_MISSILE_BARRIER | COLLIDE_WALL },
};
static_assert(ARRAY_SIZE(stru_6FD2E5F8) == MISSMODE_COUNT, "Missile functions table needs to match missiles modes");

// 1.13c D2Game+0x3AA20: the collide test the three unit-find callbacks share (esi=pUnit, ebx=txt, edi=pOwner,
// stack pGame, pMissile). Valid target, attackable, not just-hit when the row has NextHit, not the owner,
// then collide when there is no owner, the owner may hit the unit, or the row collides with friends.
static int32_t MISSMODE_CanCollideWithUnit(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2MissilesTxt* pMissilesTxtRecord, D2UnitStrc* pOwner)
{
    if (!pUnit || !(pUnit->dwFlags & UNITFLAG_ISVALIDTARGET) || !(pUnit->dwFlags & UNITFLAG_CANBEATTACKED))
    {
        return 0;
    }

    if (pMissilesTxtRecord->nNextHit && STATES_CheckState(pUnit, STATE_JUSTHIT))
    {
        return 0;
    }

    if (MISSILE_CheckUnitIfOwner(pMissile, pUnit))
    {
        return 0;
    }

    if (!pOwner || sub_6FCBD900(pGame, pOwner, pUnit) || pMissilesTxtRecord->nCollideFriend)
    {
        return 1;
    }

    return 0;
}

//D2Game.0x6FC55CE0
// 1.13c D2Game+0x3ADD0
int32_t __fastcall MISSMODE_UnitFindCallback_CanCollideWithMonster(D2UnitStrc* pUnit, void* pArgument)
{
    D2MissileUnitFindArgStrc* pArg = (D2MissileUnitFindArgStrc*)pArgument;
    if (pArg && pArg->pMissilesTxtRecord && pUnit && pUnit->dwUnitType == UNIT_MONSTER)
    {
        return MISSMODE_CanCollideWithUnit(pArg->pGame, pArg->pMissile, pUnit, pArg->pMissilesTxtRecord, pArg->pOwner) != 0;
    }

    return 0;
}

//D2Game.0x6FC55D90
// 1.13c D2Game+0x3AD70
int32_t __fastcall MISSMODE_UnitFindCallback_CanCollideWithGoodAlignmentUnit(D2UnitStrc* pUnit, void* pArgument)
{
    D2MissileUnitFindArgStrc* pArg = (D2MissileUnitFindArgStrc*)pArgument;
    if (pArg && pArg->pMissilesTxtRecord)
    {
        const int32_t nUnitType = pUnit ? pUnit->dwUnitType : 6;
        if (nUnitType == UNIT_PLAYER || (nUnitType == UNIT_MONSTER && STATLIST_GetUnitAlignment(pUnit) == UNIT_ALIGNMENT_GOOD))
        {
            return MISSMODE_CanCollideWithUnit(pArg->pGame, pArg->pMissile, pUnit, pArg->pMissilesTxtRecord, pArg->pOwner) != 0;
        }
    }

    return 0;
}

//D2Game.0x6FC55E60
// 1.13c D2Game+0x3AD30
int32_t __fastcall MISSMODE_UnitFindCallback_CanCollideWithPlayerOrMonster(D2UnitStrc* pUnit, void* pArgument)
{
    D2MissileUnitFindArgStrc* pArg = (D2MissileUnitFindArgStrc*)pArgument;
    if (pArg && pArg->pMissilesTxtRecord && pUnit && (int32_t)pUnit->dwUnitType >= 0 && (int32_t)pUnit->dwUnitType < 2)
    {
        return MISSMODE_CanCollideWithUnit(pArg->pGame, pArg->pMissile, pUnit, pArg->pMissilesTxtRecord, pArg->pOwner) != 0;
    }

    return 0;
}

//D2Game.0x6FC55F20
// 1.13c D2Game+0x39510: the argument is unused; the missiles.txt lookup is the inlined range check
int32_t __fastcall MISSMODE_UnitFindCallback_CanMissileDestroy(D2UnitStrc* pUnit, void* pArgument)
{
    D2_MAYBE_UNUSED(pArgument);

    if (pUnit && pUnit->dwUnitType == UNIT_MISSILE)
    {
        const int32_t nMissileId = (int32_t)pUnit->dwClassId;
        if (nMissileId >= 0 && nMissileId < sgptDataTables->nMissilesTxtRecordCount)
        {
            D2MissilesTxt* pMissilesTxtRecord = &sgptDataTables->pMissilesTxt[nMissileId];
            if ((uint8_t)pMissilesTxtRecord->dwMissileFlags & (uint8_t)gdwBitMasks[MISSILESFLAGINDEX_CANDESTROY])
            {
                return 1;
            }
        }
    }

    return 0;
}

//D2Game.0x6FC55F80
// 1.13c D2Game+0x3A4E0
void __stdcall MISSMODE_FillDamageParams(D2UnitStrc* pMissile, D2UnitStrc* pTarget, D2DamageStrc* pDamage)
{
    memset(pDamage, 0x00, sizeof(*pDamage));

    pDamage->dwPhysDamage = MISSMODE_RollDamageValue(pMissile, STAT_MINDAMAGE, STAT_MAXDAMAGE, -1);
    pDamage->dwFireDamage = MISSMODE_RollDamageValue(pMissile, STAT_FIREMINDAM, STAT_FIREMAXDAM, STAT_PASSIVE_FIRE_MASTERY);
    pDamage->dwMagDamage = MISSMODE_RollDamageValue(pMissile, STAT_MAGICMINDAM, STAT_MAGICMAXDAM, STAT_PASSIVE_MAG_MASTERY);
    pDamage->dwLtngDamage = MISSMODE_RollDamageValue(pMissile, STAT_LIGHTMINDAM, STAT_LIGHTMAXDAM, STAT_PASSIVE_LTNG_MASTERY);
    pDamage->dwColdDamage = MISSMODE_RollDamageValue(pMissile, STAT_COLDMINDAM, STAT_COLDMAXDAM, STAT_PASSIVE_COLD_MASTERY);
    pDamage->dwColdLen = STATLIST_UnitGetStatValue(pMissile, STAT_COLDLENGTH, 0);
    pDamage->dwPoisDamage = MISSMODE_RollDamageValue(pMissile, STAT_POISONMINDAM, STAT_POISONMAXDAM, STAT_PASSIVE_POIS_MASTERY);
    pDamage->dwPoisLen = STATLIST_UnitGetStatValue(pMissile, STAT_POISONLENGTH, 0);

    const int32_t nPoisonCount = STATLIST_UnitGetStatValue(pMissile, STAT_POISON_COUNT, 0);
    if (nPoisonCount > 1)
    {
        pDamage->dwPoisLen /= nPoisonCount;
    }

    pDamage->dwManaLeech = STATLIST_UnitGetStatValue(pMissile, STAT_MANADRAINMINDAM, 0);
    pDamage->dwLifeLeech = STATLIST_UnitGetStatValue(pMissile, STAT_LIFEDRAINMINDAM, 0);
    pDamage->dwStamLeech = STATLIST_UnitGetStatValue(pMissile, STAT_STAMDRAINMINDAM, 0);
    pDamage->dwBurnDamage = MISSMODE_RollDamageValue(pMissile, STAT_BURNINGMIN, STAT_BURNINGMAX, STAT_PASSIVE_FIRE_MASTERY);
    pDamage->dwBurnLen = STATLIST_UnitGetStatValue(pMissile, STAT_FIRELENGTH, 0);
    pDamage->dwStunLen = STATLIST_UnitGetStatValue(pMissile, STAT_STUNLENGTH, 0);

    if (pTarget)
    {
        int32_t nDamagePercent = STATLIST_UnitGetStatValue(pMissile, STAT_DAMAGEPERCENT, 0);
        if (MONSTERS_IsDemon(pTarget))
        {
            nDamagePercent += STATLIST_UnitGetStatValue(pMissile, STAT_ITEM_DEMONDAMAGE_PERCENT, 0);
        }

        if (MONSTERS_IsUndead(pTarget))
        {
            nDamagePercent += STATLIST_UnitGetStatValue(pMissile, STAT_ITEM_UNDEADDAMAGE_PERCENT, 0);
        }

        if (pTarget->dwUnitType == UNIT_MONSTER)
        {
            D2MonStatsTxt* pMonStatsTxt = MONSTERMODE_GetMonStatsTxtRecord(pTarget->dwClassId);
            if (pMonStatsTxt)
            {
                const int32_t nMonType = pMonStatsTxt->wMonType;
                if (nMonType > 0)
                {
                    D2StatStrc statBuffer[128] = {};
                    const int32_t nStats = STATLIST_CopyStats(pMissile, STAT_DAMAGE_VS_MONTYPE, statBuffer, std::size(statBuffer));

                    for (int32_t i = 0; i < nStats; ++i)
                    {
                        const int32_t nLayer = statBuffer[i].nLayer;
                        if (nLayer > 0 && nLayer < sgptDataTables->nMonTypeTxtRecordCount && nMonType > 0 && nMonType < sgptDataTables->nMonTypeTxtRecordCount)
                        {
                            if (gdwBitMasks[nLayer & 31] & sgptDataTables->pMonTypeNest[(nLayer >> 5) + nMonType * sgptDataTables->nMonTypeIndex])
                            {
                                nDamagePercent += statBuffer[i].nValue;
                            }
                        }
                    }
                }
            }
        }

        nDamagePercent = std::max(nDamagePercent, -90);

        pDamage->dwPhysDamage += (nDamagePercent * pDamage->dwPhysDamage) / 100;
    }

    if (STATLIST_UnitGetStatValue(pMissile, STAT_ITEM_DEADLYSTRIKE, 0))
    {
        pDamage->wResultFlags |= DAMAGERESULTFLAG_CRITICALSTRIKE;
        pDamage->dwPhysDamage *= 2;
    }

    if (STATLIST_UnitGetStatValue(pMissile, STAT_SKILL_BYPASS_UNDEAD, 0))
    {
        pDamage->dwHitFlags |= DAMAGEHITFLAG_BYPASSUNDEAD;
    }

    if (STATLIST_UnitGetStatValue(pMissile, STAT_SKILL_BYPASS_DEMONS, 0))
    {
        pDamage->dwHitFlags |= DAMAGEHITFLAG_BYPASSDEMON;
    }

    if (STATLIST_UnitGetStatValue(pMissile, STAT_SKILL_BYPASS_BEASTS, 0))
    {
        pDamage->dwHitFlags |= DAMAGEHITFLAG_BYPASSBEASTS;
    }
}

//D2Game.0x6FC56290
// 1.13c D2Game+0x39B40 (ecx=mastery stat, eax=max stat, stack pUnit, min stat): the max stat is read
// first, then the min; the larger becomes the max; a nonzero mastery raises both by its percentage
// (0x214D0); one RollLimited draw (0x211D0) over max-min on the unit seed.
int32_t __fastcall MISSMODE_RollDamageValue(D2UnitStrc* pUnit, int32_t nMinDamStat, int32_t nMaxDamStat, int32_t nMasteryStat)
{
    int32_t nMaxDam = STATLIST_UnitGetStatValue(pUnit, nMaxDamStat, 0);
    if (nMaxDam <= 0)
    {
        return 0;
    }

    int32_t nMinDam = STATLIST_UnitGetStatValue(pUnit, nMinDamStat, 0);
    if (nMinDam <= 0)
    {
        return 0;
    }

    if (nMinDam > nMaxDam)
    {
        std::swap(nMinDam, nMaxDam);
    }

    if (nMasteryStat != -1)
    {
        const int32_t nMastery = STATLIST_UnitGetStatValue(pUnit, nMasteryStat, 0);
        if (nMastery != 0)
        {
            nMinDam += MONSTERUNIQUE_CalculatePercentage(nMinDam, nMastery, 100);
            nMaxDam += MONSTERUNIQUE_CalculatePercentage(nMaxDam, nMastery, 100);
        }
    }

    return ITEMS_RollLimitedRandomNumber(&pUnit->pSeed, nMaxDam - nMinDam) + nMinDam;
}

//D2Game.0x6FC56480
// 1.13c D2Game+0x3A280 (edi=pAttacker, esi=pDamage, stack pDefender; pGame unused): switch on the
// missile's element (#10557); a physical hit adds damage%, demon% (#10255) and undead% (#10239),
// floored at -90, then doubles on deadly strike and sets the critical-strike result bit.
int32_t __fastcall MISSMODE_GetDamageValue(D2GameStrc* pGame, D2UnitStrc* pAttacker, D2UnitStrc* pDefender, D2DamageStrc* pDamage)
{
    D2_MAYBE_UNUSED(pGame);

    if (!pAttacker)
    {
        return 0;
    }

    switch (MISSILE_GetElemTypeFromMissileId(pAttacker->dwClassId))
    {
    case ELEMTYPE_NONE:
    {
        pDamage->dwPhysDamage = MISSMODE_RollDamageValue(pAttacker, STAT_MINDAMAGE, STAT_MAXDAMAGE, -1);

        if (pDefender)
        {
            int32_t nDamagePercent = STATLIST_UnitGetStatValue(pAttacker, STAT_DAMAGEPERCENT, 0);
            if (MONSTERS_IsDemon(pDefender))
            {
                nDamagePercent += STATLIST_UnitGetStatValue(pAttacker, STAT_ITEM_DEMONDAMAGE_PERCENT, 0);
            }

            if (MONSTERS_IsUndead(pDefender))
            {
                nDamagePercent += STATLIST_UnitGetStatValue(pAttacker, STAT_ITEM_UNDEADDAMAGE_PERCENT, 0);
            }

            if (nDamagePercent < -89)
            {
                nDamagePercent = -90;
            }

            pDamage->dwPhysDamage = (pDamage->dwPhysDamage * nDamagePercent) / 100 + pDamage->dwPhysDamage;
        }

        if (STATLIST_UnitGetStatValue(pAttacker, STAT_ITEM_DEADLYSTRIKE, 0))
        {
            pDamage->wResultFlags |= DAMAGERESULTFLAG_CRITICALSTRIKE;
            pDamage->dwPhysDamage *= 2;
        }

        return pDamage->dwPhysDamage;
    }
    case ELEMTYPE_FIRE:
    {
        pDamage->dwFireDamage = MISSMODE_RollDamageValue(pAttacker, STAT_FIREMINDAM, STAT_FIREMAXDAM, STAT_PASSIVE_FIRE_MASTERY);
        return pDamage->dwFireDamage;
    }
    case ELEMTYPE_LTNG:
    {
        pDamage->dwLtngDamage = MISSMODE_RollDamageValue(pAttacker, STAT_LIGHTMINDAM, STAT_LIGHTMAXDAM, STAT_PASSIVE_LTNG_MASTERY);
        return pDamage->dwLtngDamage;
    }
    case ELEMTYPE_MAGIC:
    {
        pDamage->dwMagDamage = MISSMODE_RollDamageValue(pAttacker, STAT_MAGICMINDAM, STAT_MAGICMAXDAM, STAT_PASSIVE_MAG_MASTERY);
        return pDamage->dwMagDamage;
    }
    case ELEMTYPE_COLD:
    {
        pDamage->dwColdDamage = MISSMODE_RollDamageValue(pAttacker, STAT_COLDMINDAM, STAT_COLDMAXDAM, STAT_PASSIVE_COLD_MASTERY);
        pDamage->dwColdLen = STATLIST_UnitGetStatValue(pAttacker, STAT_COLDLENGTH, 0);
        return pDamage->dwColdDamage;
    }
    case ELEMTYPE_POIS:
    {
        pDamage->dwPoisDamage = MISSMODE_RollDamageValue(pAttacker, STAT_POISONMINDAM, STAT_POISONMAXDAM, STAT_PASSIVE_POIS_MASTERY);
        pDamage->dwPoisLen = STATLIST_UnitGetStatValue(pAttacker, STAT_POISONLENGTH, 0);

        const int32_t nPoisonCount = STATLIST_UnitGetStatValue(pAttacker, STAT_POISON_COUNT, 0);
        if (nPoisonCount > 1)
        {
            pDamage->dwPoisLen /= nPoisonCount;
        }
        return 25 * pDamage->dwPoisDamage;
    }
    case ELEMTYPE_LIFE:
    {
        pDamage->dwLifeLeech = STATLIST_UnitGetStatValue(pAttacker, STAT_LIFEDRAINMINDAM, 0);
        break;
    }
    case ELEMTYPE_MANA:
    {
        pDamage->dwManaLeech = STATLIST_UnitGetStatValue(pAttacker, STAT_MANADRAINMINDAM, 0);
        return 0;
    }
    case ELEMTYPE_STAMINA:
    {
        pDamage->dwStamLeech = STATLIST_UnitGetStatValue(pAttacker, STAT_STAMDRAINMINDAM, 0);
        return 0;
    }
    case ELEMTYPE_STUN:
    {
        pDamage->dwStunLen = STATLIST_UnitGetStatValue(pAttacker, STAT_STUNLENGTH, 0);
        return 0;
    }
    case ELEMTYPE_BURN:
    {
        pDamage->dwBurnDamage = MISSMODE_RollDamageValue(pAttacker, STAT_BURNINGMIN, STAT_BURNINGMAX, STAT_PASSIVE_FIRE_MASTERY);
        pDamage->dwBurnLen = STATLIST_UnitGetStatValue(pAttacker, STAT_FIRELENGTH, 0);
        return 25 * pDamage->dwBurnDamage;
    }
    case ELEMTYPE_FREEZE:
    {
        pDamage->dwColdDamage = MISSMODE_RollDamageValue(pAttacker, STAT_COLDMINDAM, STAT_COLDMAXDAM, STAT_PASSIVE_COLD_MASTERY);
        pDamage->dwFrzLen = STATLIST_UnitGetStatValue(pAttacker, STAT_COLDLENGTH, 0);
        return pDamage->dwColdDamage;
    }
	default:
		break;
    }

    return 0;
}

//D2Game.0x6FC56730
// 1.13c D2Game+0x392E0 (eax=pUnit, esi=pDamage; pGame unused): clear the rolled damages, leeches,
// stun and burn damage and the three bypass bits (0x700), then the lengths the element does not keep.
void __fastcall MISSMODE_ResetDamageParams(D2GameStrc* pGame, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    D2_MAYBE_UNUSED(pGame);

    if (!pUnit)
    {
        return;
    }

    pDamage->dwPhysDamage = 0;
    pDamage->dwFireDamage = 0;
    pDamage->dwLtngDamage = 0;
    pDamage->dwMagDamage = 0;
    pDamage->dwColdDamage = 0;
    pDamage->dwPoisDamage = 0;
    pDamage->dwLifeLeech = 0;
    pDamage->dwManaLeech = 0;
    pDamage->dwStamLeech = 0;
    pDamage->dwStunLen = 0;
    pDamage->dwBurnDamage = 0;
    pDamage->dwHitFlags &= ~(DAMAGEHITFLAG_BYPASSUNDEAD | DAMAGEHITFLAG_BYPASSDEMON | DAMAGEHITFLAG_BYPASSBEASTS);

    switch (MISSILE_GetElemTypeFromMissileId(pUnit->dwClassId))
    {
    case ELEMTYPE_COLD:
    case ELEMTYPE_FREEZE:
        pDamage->dwPoisLen = 0;
        pDamage->dwBurnLen = 0;
        return;

    case ELEMTYPE_POIS:
        pDamage->dwColdLen = 0;
        pDamage->dwFrzLen = 0;
        pDamage->dwBurnLen = 0;
        return;

    case ELEMTYPE_BURN:
        pDamage->dwColdLen = 0;
        pDamage->dwPoisLen = 0;
        pDamage->dwFrzLen = 0;
        return;

    default:
        pDamage->dwColdLen = 0;
        pDamage->dwFrzLen = 0;
        pDamage->dwPoisLen = 0;
        pDamage->dwBurnLen = 0;
        return;
    }
}

//D2Game.0x6FC567E0
// 1.13c D2Game+0x39260 (checked against decompile, no delta)
void __fastcall MISSMODE_AddDamageValue(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage, int32_t nDamage)
{
    if (!pMissile)
    {
        return;
    }

    switch (D2_CALL(MISSILE_GetElemTypeFromMissileId, pMissile->dwClassId))
    {
    case ELEMTYPE_NONE:
        pDamage->dwPhysDamage += nDamage;
        break;
    case ELEMTYPE_FIRE:
        pDamage->dwFireDamage += nDamage;
        break;
    case ELEMTYPE_LTNG:
        pDamage->dwLtngDamage += nDamage;
        break;
    case ELEMTYPE_MAGIC:
        pDamage->dwMagDamage += nDamage;
        break;
    case ELEMTYPE_COLD:
        pDamage->dwColdDamage += nDamage;
        break;
    case ELEMTYPE_POIS:
        pDamage->dwPoisDamage += nDamage;
        break;
    case ELEMTYPE_LIFE:
        pDamage->dwLifeLeech += nDamage;
        break;
    case ELEMTYPE_MANA:
        pDamage->dwManaLeech += nDamage;
        break;
    case ELEMTYPE_STAMINA:
        pDamage->dwStamLeech += nDamage;
        break;
    case ELEMTYPE_STUN:
        pDamage->dwStunLen += nDamage;
        break;
    case ELEMTYPE_BURN:
        pDamage->dwBurnDamage += nDamage;
        break;
    case ELEMTYPE_FREEZE:
        pDamage->dwColdDamage = nDamage;
        break;
	default:
		break;
    }
}

//D2Game.0x6FC568F0
// 1.13c D2Game+0x3D680 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_CreatePoisonCloudHitSubmissiles(D2GameStrc* pGame, D2UnitStrc* pOwner, D2UnitStrc* pOrigin, int32_t nMissileId, int32_t nSkillId, int32_t nSkillLevel, int32_t nSubStep, int32_t nMainStep, int32_t nLoops)
{
    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(nMissileId);
    if (!pMissilesTxtRecord || !pOwner || !pOrigin)
    {
        return 0;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 0x17;
    if (nLoops > 0)
    {
        missileParams.dwFlags = 0x1F;
        missileParams.nLoops = nLoops;
    }

    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pOrigin;
    missileParams.nMissile = nMissileId;
    missileParams.nX = CLIENTS_GetUnitX(pOrigin);
    missileParams.nY = CLIENTS_GetUnitY(pOrigin);
    missileParams.nSkill = nSkillId;
    missileParams.nSkillLevel = nSkillLevel;
    missileParams.nVelocity = pMissilesTxtRecord->dwParam[0] << 7;

    nMainStep = std::max(nMainStep, 1);

    constexpr int32_t xOffsets[16] =
    {
        0, 1, 2, 2, 2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1
    };

    constexpr int32_t yOffsets[16] =
    {
        2, 2, 2, 1, 0, -1, -2, -2, -2, -2, -2, -1, 0, 1, 2, 2
    };

    for (int32_t i = 0; i < 16; i += nMainStep)
    {
        missileParams.nTargetX = xOffsets[i];
        missileParams.nTargetY = yOffsets[i];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    if (nSubStep)
    {
        missileParams.nVelocity = pMissilesTxtRecord->dwParam[1] << 7;

        for (int32_t i = 0; i < 15; i += nSubStep)
        {
            missileParams.nTargetX = xOffsets[i + 1];
            missileParams.nTargetY = yOffsets[i + 1];
            MISSILES_CreateMissileFromParams(pGame, &missileParams);
        }
    }

    return 1;
}

//D2Game.0x6FC56AB0
// 1.13c D2Game+0x3D450 (stack game, unit, radius, missile, range; ret 0x14): over the disc of the
// radius, a cell the ray from it outward does not block (#11076, mask 4) and whose room (0x1280,
// looked up from the unit's room again) is not in town (#10057) gets a submissile; a nonzero range
// sets request flag 0x8000 and the range. Retail leaves the radius in EAX; no caller reads it.
void __fastcall MISSMODE_CreateImmolationArrowHitSubmissiles(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t a3, int32_t nMissileId, int32_t nRange)
{
    if (nRange < 0)
    {
        nRange = 0;
    }

    if (!UNITS_GetRoom(pUnit))
    {
        return;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = nRange ? 0x8001 : 1;
    missileParams.pOwner = SUNIT_GetOwner(pGame, pUnit);
    missileParams.nMissile = nMissileId;
    missileParams.nSkill = MISSILE_GetSkill(pUnit);
    missileParams.nSkillLevel = MISSILE_GetLevel(pUnit);

    const int32_t nUnitX = CLIENTS_GetUnitX(pUnit);
    const int32_t nUnitY = CLIENTS_GetUnitY(pUnit);
    for (int32_t x = -a3; x <= a3; ++x)
    {
        for (int32_t y = -a3; y <= a3; ++y)
        {
            const int32_t nX = nUnitX + x;
            const int32_t nY = nUnitY + y;
            D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pUnit);
            if (pRoom && x * x + y * y <= a3 * a3)
            {
                D2CoordStrc coord1 = {};
                coord1.nX = nX;
                coord1.nY = nY;

                D2CoordStrc coord2 = {};
                coord2.nX = nX + x;
                coord2.nY = nY + y;

                if (!COLLISION_RayTrace(pRoom, &coord1, &coord2, 4u))
                {
                    D2ActiveRoomStrc* pTargetRoom = D2GAME_GetRoom_6FC52070(UNITS_GetRoom(pUnit), nX, nY);
                    if (pTargetRoom && !DUNGEON_IsRoomInTown(pTargetRoom))
                    {
                        if (nRange)
                        {
                            missileParams.nRange = nRange;
                        }

                        missileParams.nX = nX;
                        missileParams.nY = nY;
                        MISSILES_CreateMissileFromParams(pGame, &missileParams);
                    }
                }
            }
        }
    }
}

//D2Game.0x6FC56D50
// 1.13c D2Game+0x3F320
int32_t __fastcall MISSMODE_HandleMissileCollision(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
    }
    
    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
    }

    if (PATH_GetVelocity(pMissile->pDynamicPath) && sub_6FCBC930(pGame, pMissile) == 2)
    {
        return MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
    }

    const int32_t nFrame = MISSILE_GetCurrentFrame(pMissile) - 1;
    MISSILE_SetCurrentFrame(pMissile, nFrame);

    if (nFrame <= 0)
    {
        return MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
    }

    const int32_t nMissileMode = pMissile->dwMissileMode;
    if (nMissileMode == MISSMODE_NOCOLLIDE)
    {
        return 1;
    }

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);
    if (!pRoom || !pMissile->pDynamicPath)
    {
        return 2;
    }

    const int32_t nFlags = D2Common_10201(pMissile->pDynamicPath);
    if (nFlags & 5)
    {
        return 2;
    }

    if (nMissileMode == MISSMODE_BARRIERCOLLIDE || MISSILE_GetCurrentFrame(pMissile) > MISSILE_GetActivateFrame(pMissile) || !nFlags)
    {
        return 1;
    }

    if (!stru_6FD2E5F8[nMissileMode].pfUnitFindCallback)
    {
        return 2;
    }

    D2MissileUnitFindArgStrc unitFindArg = {};
    unitFindArg.pGame = pGame;
    unitFindArg.pMissile = pMissile;
    unitFindArg.pOwner = SUNIT_GetOwner(pGame, pMissile);
    unitFindArg.pMissilesTxtRecord = pMissilesTxtRecord;

    const int32_t nSize = UNITS_GetUnitSizeX(pMissile);

    D2PathPointStrc* pathPoints = nullptr;
    const int32_t nPathPoints = D2COMMON_10198_PathGetSaveStep(pMissile->pDynamicPath, &pathPoints);

    for (int32_t i = 0; i < nPathPoints; ++i)
    {
        const uint16_t nX = pathPoints[i].X;
        const uint16_t nY = pathPoints[i].Y;
        const uint16_t nCollisionMask = COLLISION_CheckMaskWithSize(pRoom, nX, nY, nSize, stru_6FD2E5F8[nMissileMode].nCollisionMask);
        if (nCollisionMask)
        {
            D2UnitStrc* pTarget = D2Common_10407(pRoom, nX, nY, stru_6FD2E5F8[nMissileMode].pfUnitFindCallback, &unitFindArg, nSize);
            if (pTarget)
            {
                return MISSMODE_SrvDmgHitHandler(pGame, pMissile, pTarget, 0);
            }

            if (nCollisionMask & 4)
            {
                MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
                return 2;
            }
        }
    }

    return 1;
}

//D2Game.0x6FC56FA0
// 1.13c D2Game+0x3D320 (checked against decompile+disassembly, no delta)
D2UnitStrc* __fastcall MISSMODE_CreatePlagueJavelin_PoisonJavelin_PoisonTrapHitSubmissiles(D2GameStrc* pGame, D2UnitStrc* pOrigin, int32_t nMissileId, int32_t nRange, int32_t nLoops)
{
    if (!D2_CALL(D2Common_10234, pOrigin->pDynamicPath))
    {
        return nullptr;
    }

    D2MissileStrc missileParams = {};

    missileParams.dwFlags = 5;
    if (nRange > 0)
    {
        missileParams.nRange = nRange;
        missileParams.dwFlags = 0x8005;
    }

    if (nLoops > 0)
    {
        missileParams.nLoops = nLoops;
        missileParams.dwFlags |= 8;
    }

    missileParams.pOwner = SUNIT_GetOwner(pGame, pOrigin);
    if (!missileParams.pOwner)
    {
        return nullptr;
    }

    missileParams.pOrigin = pOrigin;
    missileParams.nMissile = nMissileId;
    missileParams.nX = D2_CALL(CLIENTS_GetUnitX, pOrigin);
    missileParams.nY = D2_CALL(CLIENTS_GetUnitY, pOrigin);
    missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pOrigin);
    const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pOrigin);
    missileParams.nLoops = 2 * (nLevel - 1);
    missileParams.nSkillLevel = nLevel;

    return D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
}

//D2Game.0x6FC570B0
// 1.13c D2Game+0x41B80 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo02_PlagueJavelin_PoisonJavelin_PoisonTrap(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (pMissile)
    {
        D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
        if (pMissilesTxtRecord && pMissilesTxtRecord->wSubMissile[0] >= 0)
        {
            const int32_t nLevel = MISSILE_GetLevel(pMissile);
            D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
            const int32_t nValue = MISSILE_EvaluateMissileFormula(pMissile, pOwner, pMissilesTxtRecord->dwSrvCalc, pMissile->dwClassId, nLevel);
            MISSMODE_CreatePlagueJavelin_PoisonJavelin_PoisonTrapHitSubmissiles(pGame, pMissile, pMissilesTxtRecord->wSubMissile[0], 0, nValue);
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57140
// 1.13c D2Game+0x41AE0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo03_PoisonCloud_Blizzard_ThunderStorm_HandOfGod(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile->pDynamicPath || !PATH_GetVelocity(pMissile->pDynamicPath))
    {
        COLLISION_SetMask(UNITS_GetRoom(pMissile), CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), 0x40);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC571D0
// 1.13c D2Game+0x41980 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo05_FireWall_ImmolationFire_MeteorFire_MoltenBoulderFirePath(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return MISSMODE_HandleMissileCollision(pGame, pMissile);
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return MISSMODE_HandleMissileCollision(pGame, pMissile);
    }

    const int32_t nCurrentFrame = pMissile->nSeqCurrentFramePrecise >> 8;
    COLLISION_SetMaskWithSize(UNITS_GetRoom(pMissile), CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), UNITS_GetUnitSizeX(pMissile), 0x40);

    if (nCurrentFrame == pMissilesTxtRecord->nSubStart - 1)
    {
        pMissile->nSeqCurrentFramePrecise = (nCurrentFrame + ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, pMissilesTxtRecord->nSubStop - pMissilesTxtRecord->nSubStart)) << 8;
    }
    else if (MISSILE_GetCurrentFrame(pMissile) == pMissilesTxtRecord->nSubStart)
    {
        pMissile->nSeqCurrentFramePrecise = (pMissilesTxtRecord->nSubStart - 3) << 8;
    }
    else if (MISSILE_GetCurrentFrame(pMissile) < pMissilesTxtRecord->nSubStart)
    {
        const int32_t nCappedFrame = std::max(nCurrentFrame - 2, 0);
        pMissile->nSeqCurrentFramePrecise = nCappedFrame << 8;
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC573B0
// 1.13c D2Game+0x41850: the owner (stored-owner bit, 0xE03A0) first, then the missiles.txt row, its
// first submissile and the path; a step that lands on a new tile (#10203) spawns the submissile there
// (request 0x21, target = position); every miss returns 2.
int32_t __fastcall MISSMODE_SrvDo06_MoltenBoulder_FireWallMaker(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0 || !pMissile->pDynamicPath)
    {
        return 2;
    }

    if (D2Common_10234(pMissile->pDynamicPath))
    {
        const int32_t nX = CLIENTS_GetUnitX(pMissile);
        const int32_t nY = CLIENTS_GetUnitY(pMissile);

        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 0x21;
        missileParams.pOwner = pOwner;
        missileParams.nX = nX;
        missileParams.nY = nY;
        missileParams.nTargetX = nX;
        missileParams.nTargetY = nY;
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57510
// 1.13c D2Game+0x41710: row, then a live owner (stored-owner bit, 0xE03A0, 0xDFA90) outside town;
// when the homing bit of target-x is set the target is refreshed (0xE07A0) and dropped if dead or not
// hostile to the owner (0xD00A60); every Param1-th frame (default 5) at range 4..24 the path re-aims
// (#10334).
int32_t __fastcall MISSMODE_SrvDo07_GuidedArrow_BoneSpirit(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || SUNIT_IsDead(pOwner) || DUNGEON_IsRoomInTown(UNITS_GetRoom(pMissile)))
    {
        return 2;
    }

    if (MISSILE_GetTargetX(pMissile) & 1)
    {
        sub_6FCBC7E0(pGame, pMissile);

        D2DynamicPathStrc* pPath = pMissile->pDynamicPath;
        D2UnitStrc* pTarget = SUNIT_GetTargetUnit(pGame, pMissile);
        if (pTarget && (SUNIT_IsDead(pTarget) || !sub_6FCBD900(pGame, pOwner, pTarget)))
        {
            pTarget = nullptr;
        }

        int32_t nParam = pMissilesTxtRecord->dwParam[0];
        if (nParam < 1)
        {
            nParam = 5;
        }

        if (pTarget && MISSILE_GetCurrentFrame(pMissile) % nParam == 0)
        {
            const int32_t nDistance = UNITS_GetDistanceToOtherUnit(pMissile, pTarget);
            if (nDistance > 3 && nDistance < 25)
            {
                D2Common_10142(pPath, pMissile, 0);
            }
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57640
// 1.13c D2Game+0x41670: level / max(Param3, 1) gives the range bonus (at least 2) over Param1 and the
// frame step Param2 minus it (at least 3); spawn through 0x3D1E0 with mask 5, then the collision step.
// Retail dereferences a null row (reads its submissile); ours returns 2 there.
int32_t __fastcall MISSMODE_SrvDo08_MonBlizzCenter(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    int32_t nDivisor = pMissilesTxtRecord->dwParam[2];
    if (nDivisor < 1)
    {
        nDivisor = 1;
    }

    const int32_t nDividedLevel = MISSILE_GetLevel(pMissile) / nDivisor;
    const int32_t nRangeBonus = nDividedLevel < 2 ? 2 : nDividedLevel;
    const int32_t nRange = pMissilesTxtRecord->dwParam[0] + nRangeBonus;
    int32_t nFrames = pMissilesTxtRecord->dwParam[1] - nDividedLevel;
    if (nFrames < 3)
    {
        nFrames = 3;
    }

    MISSMODE_CreateMissileWithCollisionCheck(pGame, pMissile, nRange, nFrames, pMissilesTxtRecord->wSubMissile[0], 5);
    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC576F0
// 1.10f body; the 1.13c 0x3D1E0 (EAX = nFrames, EBX = missile, stack game/range/missile/mask) is src/core/d2game/d2g_3d1e0.cpp
D2UnitStrc* __fastcall MISSMODE_CreateMissileWithCollisionCheck(D2GameStrc* pGame, D2UnitStrc* pMissile, int32_t nRange, int32_t nFrames, int32_t nMissileId, uint16_t nCollisionMask)
{
    if (!nFrames || (MISSILE_GetRemainingFrames(pMissile) % nFrames) != 0)
    {
        return nullptr;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || !UNITS_GetRoom(pMissile))
    {
        return nullptr;
    }

    SEED_InitLowSeed(&pMissile->pSeed, CLIENTS_GetUnitX(pMissile) + MISSILE_GetRemainingFrames(pMissile));

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 3;
    missileParams.pOwner = pOwner;
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = nMissileId;

    const int32_t nMaxRange = nRange - 1;
    missileParams.nX = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nMaxRange) - nMaxRange + CLIENTS_GetUnitX(pMissile);
    missileParams.nY = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nMaxRange) - nMaxRange + CLIENTS_GetUnitY(pMissile);

    if (COLLISION_CheckMask(UNITS_GetRoom(pMissile), missileParams.nX, missileParams.nY, nCollisionMask))
    {
        return nullptr;
    }

    return MISSILES_CreateMissileFromParams(pGame, &missileParams);
}

//D2Game.0x6FC57910
// 1.13c D2Game+0x41520
int32_t __fastcall MISSMODE_SrvDo09_BatLightningBolt(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    // retail reads the position first, then asks the path
    const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pMissile);
    const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pMissile);
    if (D2_CALL(D2Common_10234, pMissile->pDynamicPath))
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 1;
        missileParams.pOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);
        missileParams.nX = nX;
        missileParams.nY = nY;
        missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
        missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
    }

    return D2_CALL(MISSMODE_HandleMissileCollision, pGame, pMissile);
}

//D2Game.0x6FC57A40
// 1.13c D2Game+0x413D0: missiles.txt row, skills.txt row of the missile's skill, then the range is
// calc1 and the frame step calc2, each evaluated (#10284) on a fresh owner lookup, in that order; spawn
// through 0x3D1E0 with mask 5, then the collision step. Retail dereferences a null row; ours returns 2.
int32_t __fastcall MISSMODE_SrvDo10_BlizzardCenter(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    const int32_t nRange = SKILLS_EvaluateSkillFormula(SUNIT_GetOwner(pGame, pMissile), pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
    const int32_t nFrames = SKILLS_EvaluateSkillFormula(SUNIT_GetOwner(pGame, pMissile), pSkillsTxtRecord->dwCalc[1], nSkillId, nLevel);
    MISSMODE_CreateMissileWithCollisionCheck(pGame, pMissile, nRange, nFrames, pMissilesTxtRecord->wSubMissile[0], 5);
    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57B60
// 1.13c D2Game+0x41240: row, then the missile's target (0xE07A0 + #10392, itself = none) and the owner;
// with no target the owner's target (itself = none). Every Param1-th frame (default 5) a target within
// Param2 pulls the path Param3 (at least 1) tiles toward it on each axis (#10286), then re-aims (#10334).
int32_t __fastcall MISSMODE_SrvDo11_FingerMageSpider(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    D2UnitStrc* pTarget = SUNIT_GetTargetUnit(pGame, pMissile);
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pTarget && pOwner)
    {
        pTarget = SUNIT_GetTargetUnit(pGame, pOwner);
    }

    if (pTarget)
    {
        int32_t nFrames = pMissilesTxtRecord->dwParam[0];
        if (nFrames < 1)
        {
            nFrames = 5;
        }

        if (MISSILE_GetCurrentFrame(pMissile) % nFrames == 0 && UNITS_GetDistanceToOtherUnit(pMissile, pTarget) <= pMissilesTxtRecord->dwParam[1])
        {
            const int32_t nMissileX = CLIENTS_GetUnitX(pMissile);
            const int32_t nMissileY = CLIENTS_GetUnitY(pMissile);
            const int32_t nTargetX = CLIENTS_GetUnitX(pTarget);
            const int32_t nTargetY = CLIENTS_GetUnitY(pTarget);
            int32_t nStep = pMissilesTxtRecord->dwParam[2];
            if (nStep < 1)
            {
                nStep = 1;
            }

            const int32_t nX = nTargetX > nMissileX ? nStep : (nTargetX < nMissileX ? -nStep : 0);
            const int32_t nY = nTargetY > nMissileY ? nStep : (nTargetY < nMissileY ? -nStep : 0);
            const int32_t nNewY = CLIENTS_GetUnitY(pMissile) + nY;
            const int32_t nNewX = CLIENTS_GetUnitX(pMissile) + nX;
            D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nNewX, nNewY);
            D2Common_10142(pMissile->pDynamicPath, pMissile, 0);
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57DA0
// 1.13c D2Game+0x41110: row with a first submissile, then an owner (stored-owner bit, 0xE03A0) or 2;
// a step onto a new tile (#10203) spawns the submissile there (request 0x21, owner from 0xE0A20).
int32_t __fastcall MISSMODE_SrvDo12_DiabWallMaker(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    if (!SUNIT_GetOwner(pGame, pMissile))
    {
        return 2;
    }

    if (D2Common_10234(pMissile->pDynamicPath))
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 0x21;
        missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
        missileParams.nX = CLIENTS_GetUnitX(pMissile);
        missileParams.nY = CLIENTS_GetUnitY(pMissile);
        missileParams.nTargetX = missileParams.nX;
        missileParams.nTargetY = missileParams.nY;
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC57F00
// 1.13c D2Game+0x40EB0
int32_t __fastcall MISSMODE_SrvDo13_BoneWallMaker(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!SUNIT_GetOwner(pGame, pMissile) || !MISSILE_GetTargetY(pMissile))
    {
        return 2;
    }

    if (!D2Common_10234(pMissile->pDynamicPath))
    {
        return MISSMODE_HandleMissileCollision(pGame, pMissile);
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    D2SummonArgStrc summonArg = {};
    int32_t nSpawnMode;
    const int32_t nSummonId = D2GAME_GetSummonIdFromSkill_6FD15580(pOwner, 0, nSkillId, nLevel, &nSpawnMode, 0, 0);
    if (nSummonId < 0)
    {
        return 0;
    }

    int32_t nPetType = pSkillsTxtRecord->nPetType;
    if (nPetType >= sgptDataTables->nPetTypeTxtRecordCount)
    {
        nPetType = 0;
    }

    D2UnitStrc* pMonster = SUNIT_GetServerUnit(pGame, UNIT_MONSTER, MISSILE_GetTargetX(pMissile));
    if (pMonster)
    {

        summonArg.nAiSpecialState = AISPECIALSTATE_NONE;
        summonArg.nMonMode = nSpawnMode;
        summonArg.dwFlags = 0xDu;
        summonArg.pOwner = pOwner;
        summonArg.nHcIdx = nSummonId;
        summonArg.pPosition.nX = CLIENTS_GetUnitX(pMissile);
        summonArg.pPosition.nY = CLIENTS_GetUnitY(pMissile);
        summonArg.nPetType = nPetType;
        // Walls are not pet-capped: retail keeps several alive at once (a retail recording skill-78 trace).
        summonArg.nPetMax = INT32_MAX;

        D2UnitStrc* pPet = D2GAME_SummonPet_6FD14430(pGame, &summonArg);
        if (pPet)
        {
            AIGENERAL_SetOwnerData(pGame, pPet, pMonster->dwUnitId, 1, 0, 0);
            AIGENERAL_AllocMinionList(pGame, pMonster, pPet);
            D2GAME_BOSSES_AssignUMod_6FC6FF10(pGame, pPet, 15, 0);
            D2GAME_SetSummonPassiveStats_6FD0C530(pGame, pOwner, pPet, nSkillId, nLevel, 0);
            UNITS_StoreOwner(pPet, pOwner);
            sub_6FC40280(pGame, pPet, 0, 9);
            MISSILE_SetTargetY(pMissile, MISSILE_GetTargetY(pMissile) - 1);
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58140
// 1.13c D2Game+0x40E10: every Param1-th remaining frame (at least 1, #10985) the ward pulses its skill
// (0x9E420) when skill and level are both positive, else 2; then the collision step.
int32_t __fastcall MISSMODE_SrvDo14_GrimWard(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    int32_t nParam = 1;
    if (pMissilesTxtRecord->dwParam[0] > 0)
    {
        nParam = pMissilesTxtRecord->dwParam[0];
    }

    if (MISSILE_GetRemainingFrames(pMissile) % nParam == 0)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        if (nSkillId < 1 || nLevel < 1)
        {
            return 2;
        }

        sub_6FD14400(pGame, pMissile, pMissilesTxtRecord->dwParam[1], nSkillId, nLevel);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

// 1.13c D2Game+0x40CA0 (0x6FC581F0 in 1.10f)
int32_t __fastcall MISSMODE_SrvDo15_FrozenOrb(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    constexpr int32_t xPositions[] =
    {
        30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2, 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29
    };
    
    constexpr int32_t yPositions[] =
    {
        0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2
    };

    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 2;
    }

    const int32_t nFrames = std::max(pMissilesTxtRecord->dwParam[0], 1);

    if (MISSILE_GetRemainingFrames(pMissile) % nFrames <= 0)
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 2;
        missileParams.pOwner = pOwner;
        missileParams.pOrigin = pMissile;
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];

        const int32_t nIndex = std::abs(MISSILE_GetTargetX(pMissile) % 64);
        missileParams.nTargetX = xPositions[nIndex];
        missileParams.nTargetY = yPositions[nIndex];
        MISSILE_SetTargetX(pMissile, (nIndex + pMissilesTxtRecord->dwParam[1]) % 64);
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58340
// 1.13c D2Game+0x40BD0: below Param1 remaining frames, every Param2-th (at least 1) the stored offset
// (target x/y) turns 45 degrees: x' = (x-y)/2, y' = (x+y)/2; the path re-aims at position + offset
// (#10286, #10334) and the new offset is stored (#10637, #10019); then the collision step.
int32_t __fastcall MISSMODE_SrvDo16_FrozenOrbNova(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    int32_t nParam = 1;
    if (pMissilesTxtRecord->dwParam[1] > 0)
    {
        nParam = pMissilesTxtRecord->dwParam[1];
    }

    const int32_t nRemainingFrames = MISSILE_GetRemainingFrames(pMissile);
    if (nRemainingFrames < pMissilesTxtRecord->dwParam[0] && nRemainingFrames % nParam == 0)
    {
        const int32_t nTargetX = MISSILE_GetTargetX(pMissile);
        const int32_t nTargetY = MISSILE_GetTargetY(pMissile);
        const int32_t nOffsetX = (nTargetX - nTargetY) / 2;
        const int32_t nOffsetY = (nTargetX + nTargetY) / 2;
        const int32_t nNewY = CLIENTS_GetUnitY(pMissile) + nOffsetY;
        const int32_t nNewX = CLIENTS_GetUnitX(pMissile) + nOffsetX;

        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nNewX, nNewY);
        D2Common_10142(pMissile->pDynamicPath, pMissile, 0);
        MISSILE_SetTargetX(pMissile, nOffsetX);
        MISSILE_SetTargetY(pMissile, nOffsetY);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58480
// 1.13c D2Game+0x40B20: between Param1 and Range-Param1 frames a positive first submissile spawns
// through 0x3D1E0 (frame step Param2, range Param3, mask 5); then, once (target-x 0) and while the frame
// is within Param1+Param5, a non-negative Param4 opens the portal: retail's helper 0x3BDA0 (set
// target-x 1, owner, x/y, CreatePortalObject 0xA24C0, room portal flag), inlined here in its order.
int32_t __fastcall MISSMODE_SrvDo17_CairnStones(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    const int32_t nCurrentFrame = MISSILE_GetCurrentFrame(pMissile);
    if (pMissilesTxtRecord->wSubMissile[0] > 0 && nCurrentFrame < pMissilesTxtRecord->wRange - pMissilesTxtRecord->dwParam[0] && nCurrentFrame > pMissilesTxtRecord->dwParam[0])
    {
        MISSMODE_CreateMissileWithCollisionCheck(pGame, pMissile, pMissilesTxtRecord->dwParam[2], pMissilesTxtRecord->dwParam[1], pMissilesTxtRecord->wSubMissile[0], 5);
    }

    if (pMissilesTxtRecord->dwParam[3] >= 0 && !MISSILE_GetTargetX(pMissile) && nCurrentFrame <= pMissilesTxtRecord->dwParam[4] + pMissilesTxtRecord->dwParam[0])
    {
        MISSILE_SetTargetX(pMissile, 1);
        D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
        const int32_t nY = CLIENTS_GetUnitY(pMissile);
        const int32_t nX = CLIENTS_GetUnitX(pMissile);
        D2GAME_CreatePortalObject_6FD13DF0(pGame, pOwner, UNITS_GetRoom(pMissile), nX, nY, pMissilesTxtRecord->dwParam[3], 0, 60u, 1);
        DUNGEON_ToggleHasPortalFlag(UNITS_GetRoom(pMissile), 1);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC585E0
// 1.13c D2Game+0x408D0
int32_t __fastcall MISSMODE_SrvDo18_TowerChestSpawner(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    int32_t nCurrentFrame = MISSILE_GetCurrentFrame(pMissile);
    if (nCurrentFrame == 1)
    {
        D2UnitStrc* pObject = SUNIT_GetServerUnit(pGame, UNIT_OBJECT, MISSILE_GetTargetX(pMissile));
        if (pObject)
        {
            SUNIT_AttachSound(pObject, 0x5Cu, 0);
        }
    }

    if (nCurrentFrame == pMissilesTxtRecord->wRange - pMissilesTxtRecord->dwParam[0])
    {
        D2UnitStrc* pObject = SUNIT_GetServerUnit(pGame, UNIT_OBJECT, MISSILE_GetTargetX(pMissile));
        if (pObject)
        {
            D2ObjOperateFnStrc pOp = {};
            pOp.pGame = pGame;
            pOp.pObject = pObject;
            pOp.pPlayer = nullptr;
            pOp.pObjectregion = (D2ObjectControlStrc*)&pObject->pSeed;
            pOp.nObjectIdx = pObject->dwClassId;
            sub_6FC75EB0(&pOp);
        }

        MISSILE_SetTargetY(pMissile, 1);
    }

    if (MISSILE_GetTargetY(pMissile))
    {
        const int32_t nFrames = std::max(4 * pMissilesTxtRecord->dwParam[1], 1);

        if (!(nCurrentFrame % nFrames))
        {
            D2CoordStrc pCoord = {};
            UNITS_GetCoords(pMissile, &pCoord);
            const int32_t nRange = pMissilesTxtRecord->dwParam[2];
            pCoord.nX += ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nRange + 1) - nRange;
            pCoord.nY += ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nRange + 1) - nRange;

            D2CoordStrc returnCoords = {};
            D2ActiveRoomStrc* pRoom = D2GAME_GetFreeSpaceEx_6FC4BF00(UNITS_GetRoom(pMissile), &pCoord, &returnCoords, 1);
            if (pRoom)
            {
                D2ItemDropStrc itemDrop = {};
                itemDrop.pSeed = 0;
                itemDrop.pUnit = pMissile;
                itemDrop.pGame = pGame;
                itemDrop.wItemFormat = pGame->wItemFormat;
                itemDrop.nSpawnType = 3;
                itemDrop.pRoom = pRoom;
                itemDrop.nId = DATATBLS_GetItemIdFromItemCode(' dlg');
                itemDrop.nX = returnCoords.nX;
                itemDrop.nY = returnCoords.nY;
                itemDrop.wUnitInitFlags = 1;
                itemDrop.nQuality = 2;
                itemDrop.nItemLvl = DATATBLS_GetMonsterLevelInArea(DUNGEON_GetLevelIdFromRoom(pRoom), pGame->nDifficulty, pGame->bExpansion);
                D2GAME_CreateItemEx_6FC4ED80(pGame, &itemDrop, 0);
            }
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58860
// 1.13c D2Game+0x40860
int32_t __fastcall MISSMODE_SrvDo20_BladeCreeper(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2UnitStrc* pOwner = pMissile ? SUNIT_GetOwner(pGame, pMissile) : nullptr;
    if (!pOwner || SUNIT_IsDead(pOwner))
    {
        MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 1);
        return 2;
    }

    // Retail 0x5A1D0: re-reads the owner, then moves the path onto it (Y, X, room in that order).
    D2UnitStrc* pPathOwner = SUNIT_GetOwner(pGame, pMissile);
    D2DynamicPathStrc* pPath = pMissile->pDynamicPath;
    if (pPathOwner && pPath)
    {
        const int32_t nY = CLIENTS_GetUnitY(pPathOwner);
        const int32_t nX = CLIENTS_GetUnitX(pPathOwner);
        D2Common_10229(pPath, pMissile, UNITS_GetRoom(pPathOwner), nX, nY);
    }

    MISSILE_SetCurrentFrame(pMissile, 10);
    MISSMODE_HandleMissileCollision(pGame, pMissile);
    return 1;
}

//D2Game.0x6FC58940
// 1.13c D2Game+0x41EB0
int32_t __fastcall MISSMODE_SrvDo21_Distraction(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    if (pMissilesTxtRecord->wSubMissile[0] && D2Common_10234(pMissile->pDynamicPath))
    {
        const int32_t nY = CLIENTS_GetUnitY(pMissile);
        const int32_t nX = CLIENTS_GetUnitX(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        const int32_t nSkill = MISSILE_GetSkill(pMissile);
        D2GAME_CreateMissile_6FD115E0(pGame, SUNIT_GetOwner(pGame, pMissile), nSkill, nLevel, pMissilesTxtRecord->wSubMissile[0], nX, nY);
    }

    // Retail tail-jumps into the blade creeper mode.
    return MISSMODE_SrvDo20_BladeCreeper(pGame, pMissile);
}

//D2Game.0x6FC58B00
// 1.13c D2Game+0x406D0
int32_t __fastcall MISSMODE_SrvDo22_LightningTrailingJavelin(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wSubMissile[0] == 0 || !pMissile->pDynamicPath)
    {
        return 2;
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    if (MISSILE_GetRemainingFrames(pMissile) < 2)
    {
        // The step direction is the path heading turned by 90 degrees.
        const int32_t nFirstX = D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath);
        const int32_t nFirstY = D2COMMON_10176_PathGetFirstPointY(pMissile->pDynamicPath);
        MISSILE_SetTargetX(pMissile, -(nFirstY - nY));
        MISSILE_SetTargetY(pMissile, nFirstX - nX);
    }

    if (D2Common_10234(pMissile->pDynamicPath))
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 11;
        missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
        missileParams.nX = nX;
        missileParams.nY = nY;
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        missileParams.nLoops = pMissilesTxtRecord->dwParam[0];
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        missileParams.nTargetX = MISSILE_GetTargetX(pMissile);
        missileParams.nTargetY = MISSILE_GetTargetY(pMissile);
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
        missileParams.nTargetX = -MISSILE_GetTargetX(pMissile);
        missileParams.nTargetY = -MISSILE_GetTargetY(pMissile);
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58CC0
// 1.13c D2Game+0x405E0
int32_t __fastcall MISSMODE_SrvDo23_24_SuccFireBall_FirestormMaker(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wSubMissile[0] == 0)
    {
        return 2;
    }

    if (D2Common_10234(pMissile->pDynamicPath))
    {
        D2MissileStrc missileParams = {};
        const int32_t nLoops = pMissilesTxtRecord->dwParam[0];
        missileParams.dwFlags = 1;
        if (nLoops > 0)
        {
            missileParams.dwFlags = 9;
            missileParams.nLoops = nLoops;
        }
        missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
        missileParams.nX = CLIENTS_GetUnitX(pMissile);
        missileParams.nY = CLIENTS_GetUnitY(pMissile);
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58E10 (1.10f)
// 1.13c D2Game+0x404D0
int32_t __fastcall MISSMODE_SrvDo25_EruptionCenter(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wSubMissile[0] == 0)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    const int32_t nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
    const int32_t nFrames = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[1], nSkillId, nLevel);
    MISSMODE_CreateMissileWithCollisionCheck(pGame, pMissile, nRange, nFrames, pMissilesTxtRecord->wSubMissile[0], 0x45);

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC58F30
// 1.13c D2Game+0x40420
int32_t __fastcall MISSMODE_SrvDo26_Vines_PlagueVines(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wSubMissile[0] == 0)
    {
        return 2;
    }

    const int32_t nFrames = MISSILE_GetRemainingFrames(pMissile);
    int32_t nPeriod = pMissilesTxtRecord->dwParam[0];
    if (nPeriod < 1)
    {
        nPeriod = 1;
    }

    if (nFrames % nPeriod == 0)
    {
        const int32_t nY = CLIENTS_GetUnitY(pMissile);
        const int32_t nX = CLIENTS_GetUnitX(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        const int32_t nSkill = MISSILE_GetSkill(pMissile);
        D2GAME_CreateMissile_6FD115E0(pGame, SUNIT_GetOwner(pGame, pMissile), nSkill, nLevel, pMissilesTxtRecord->wSubMissile[0], nX, nY);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC59040
// 1.13c D2Game+0x40280
int32_t __fastcall MISSMODE_SrvDo27_Tornado(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    const int32_t nRemainingFrames = MISSILE_GetRemainingFrames(pMissile);

    int32_t nPeriod = pMissilesTxtRecord->dwParam[0];
    if (nPeriod <= 0)
    {
        nPeriod = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[3], nSkillId, nLevel);
        if (nPeriod < 1)
        {
            nPeriod = 1;
        }
    }

    if (nRemainingFrames % nPeriod == 0)
    {
        D2DamageStrc damage = {};
        MISSMODE_FillDamageParams(pMissile, 0, &damage);

        int32_t nRange = pMissilesTxtRecord->dwParam[1];
        if (nRange <= 0)
        {
            nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel);
            if (nRange < 1)
            {
                nRange = 1;
            }
        }

        damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
        damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;

        // Retail inlines the aura sweep wrapper here (filter 0 means the default 0x8583).
        uint32_t nAuraFilter = pSkillsTxtRecord->dwAuraFilter;
        if (!nAuraFilter)
        {
            nAuraFilter = 0x8583u;
        }
        sub_6FD0FE80(pGame, pMissile, 0, 0, nRange, nAuraFilter, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

// 1.13c D2Game+0x40070 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo28_Volcano(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner)
    {
        int32_t nAuraRange = pMissilesTxtRecord->dwParam[1];
        if (nAuraRange <= 0)
        {
            nAuraRange = std::max(SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel), 1);
        }

        int32_t nFrames = pMissilesTxtRecord->dwParam[0];
        if (nFrames <= 0)
        {
            nFrames = std::max(SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[3], nSkillId, nLevel), 1);
        }

        const int32_t nRemainingFrames = MISSILE_GetRemainingFrames(pMissile);
        if (nRemainingFrames > pMissilesTxtRecord->dwParam[2] && nRemainingFrames < pMissilesTxtRecord->dwParam[3] && !(nRemainingFrames % nFrames))
        {
            SEED_InitLowSeed(&pMissile->pSeed, MISSILE_GetTargetX(pMissile));
            const int32_t nOffsetX = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nAuraRange + 1) - nAuraRange;
            const int32_t nOffsetY = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 2 * nAuraRange + 1) - nAuraRange;
            MISSILE_SetTargetX(pMissile, SEED_GetLowSeed(&pMissile->pSeed));

            D2MissileStrc missileParams = {};

            missileParams.dwFlags = 1312;
            missileParams.pOwner = pOwner;
            missileParams.pOrigin = pMissile;
            missileParams.nTargetX = nOffsetX + CLIENTS_GetUnitX(pMissile);
            missileParams.nTargetY = nOffsetY + CLIENTS_GetUnitY(pMissile);
            missileParams.nSkill = nSkillId;
            missileParams.nGfxArg = pMissilesTxtRecord->dwParam[4];
            missileParams.nSkillLevel = nLevel;
            missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
            MISSILES_CreateMissileFromParams(pGame, &missileParams);
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC594B0
// 1.13c D2Game+0x3FED0
int32_t __fastcall MISSMODE_SrvDo29_RecyclerDelay(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner && !SUNIT_IsDead(pOwner))
    {
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        if (MISSILE_GetRemainingFrames(pMissile) == pMissilesTxtRecord->dwParam[0])
        {
            const int32_t nHitpoints = STATLIST_UnitGetStatValue(pOwner, STAT_HITPOINTS, 0) >> 8;
            const int32_t nMaxHp = STATLIST_GetMaxLifeFromUnit(pOwner) >> 8;
            if (nHitpoints < nMaxHp)
            {
                const int32_t nPercent = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
                int32_t nNewHp = nHitpoints + MONSTERUNIQUE_CalculatePercentage(nMaxHp, nPercent, 100);
                if (nNewHp >= nMaxHp)
                {
                    nNewHp = nMaxHp;
                }
                STATLIST_SetUnitStat(pOwner, STAT_HITPOINTS, nNewHp << 8, 0);

                if (pMissilesTxtRecord->wProgOverlay > 0 && pMissilesTxtRecord->wProgOverlay < sgptDataTables->nOverlayTxtRecordCount)
                {
                    UNITS_SetOverlay(pOwner, pMissilesTxtRecord->wProgOverlay, 0);
                }
            }
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC596C0
// 1.13c D2Game+0x3FD30
int32_t __fastcall MISSMODE_SrvDo33_VineRecyclerDelay(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner && !SUNIT_IsDead(pOwner))
    {
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        if (MISSILE_GetRemainingFrames(pMissile) == pMissilesTxtRecord->dwParam[0])
        {
            const int32_t nMana = STATLIST_UnitGetStatValue(pOwner, STAT_MANA, 0) >> 8;
            const int32_t nMaxMana = STATLIST_GetMaxManaFromUnit(pOwner) >> 8;
            if (nMana < nMaxMana)
            {
                const int32_t nPercent = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
                int32_t nNewMana = nMana + MONSTERUNIQUE_CalculatePercentage(nMaxMana, nPercent, 100);
                if (nNewMana >= nMaxMana)
                {
                    nNewMana = nMaxMana;
                }
                STATLIST_SetUnitStat(pOwner, STAT_MANA, nNewMana << 8, 0);

                if (pMissilesTxtRecord->wProgOverlay > 0 && pMissilesTxtRecord->wProgOverlay < sgptDataTables->nOverlayTxtRecordCount)
                {
                    UNITS_SetOverlay(pOwner, pMissilesTxtRecord->wProgOverlay, 0);
                }
            }
        }
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

// Retail 0x3A1D0: re-reads the owner, then moves the path onto it (Y, X, room in that order).
// 1.13c D2Game+0x3A1D0
void __fastcall MISSMODE_MoveMissileToOwner(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2UnitStrc* pPathOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);
    D2DynamicPathStrc* pPath = pMissile->pDynamicPath;
    if (pPathOwner && pPath)
    {
        const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pPathOwner);
        const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pPathOwner);
        D2Common_10229(pPath, pMissile, D2_CALL(UNITS_GetRoom, pPathOwner), nX, nY);
    }
}

//D2Game.0x6FC598D0
// 1.13c D2Game+0x3FB40
int32_t __fastcall MISSMODE_SrvDo30_RabiesPlague(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);

    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 2;
    }

    const int32_t nUnitGUID = D2_CALL(MISSILE_GetTargetY, pMissile);
    const int32_t nUnitType = D2_CALL(MISSILE_GetTargetX, pMissile);
    D2UnitStrc* pUnit = D2_CALL(SUNIT_GetServerUnit, pGame, nUnitType, nUnitGUID);
    D2UnitStrc* pOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);

    if (!pUnit || !pOwner || D2_CALL(SUNIT_IsDead, pOwner) || D2_CALL(MISSILE_GetCurrentFrame, pMissile) < 0)
    {
        D2_CALL(MISSMODE_SrvDmgHitHandler, pGame, pMissile, nullptr, 1);
        return 2;
    }

    D2_CALL(MISSMODE_MoveMissileToOwner, pGame, pMissile);

    const int32_t nParam = std::max(pMissilesTxtRecord->dwParam[0], 1);

    if (!(D2_CALL(MISSILE_GetRemainingFrames, pMissile) % nParam))
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 2;
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        missileParams.pOwner = pUnit;
        missileParams.pOrigin = pMissile;
        missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
        missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);

        const int32_t nRange = 2 * pMissilesTxtRecord->dwParam[1] + 1;
        missileParams.nTargetX = D2_CALL(ITEMS_RollLimitedRandomNumber, &pUnit->pSeed, nRange) - pMissilesTxtRecord->dwParam[1];
        missileParams.nTargetY = D2_CALL(ITEMS_RollLimitedRandomNumber, &pUnit->pSeed, nRange) - pMissilesTxtRecord->dwParam[1];

        D2UnitStrc* pSubMissile = D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
        if (pSkillsTxtRecord->wAuraTargetState > 0)
        {
            D2StatListStrc* pStatList = D2_CALL(STATLIST_GetStatListFromUnitAndState, pOwner, pSkillsTxtRecord->wAuraTargetState);
            if (pSubMissile && pStatList)
            {
                D2_CALL(MISSILE_SetTargetX, pSubMissile, D2_CALL(D2COMMON_10473, pStatList));
            }
        }
    }

    D2_CALL(MISSMODE_HandleMissileCollision, pGame, pMissile);

    return 1;
}

//D2Game.0x6FC59B90
// 1.13c D2Game+0x3F9F0
int32_t __fastcall MISSMODE_SrvDo31_WakeOfDestructionMaker_BaalColdMaker(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        MISSMODE_SrvDmgHitHandler(pGame, pMissile, nullptr, 0);
        return 2;
    }

    if (D2Common_10234(pMissile->pDynamicPath))
    {
        const int32_t nX = MISSILE_GetTargetX(pMissile);
        const int32_t nY = MISSILE_GetTargetY(pMissile);

        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 2;
        missileParams.pOwner = pOwner;
        missileParams.pOrigin = pMissile;
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nTargetX = nX;
        missileParams.nTargetY = nY;
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
        missileParams.nTargetX = -nX;
        missileParams.nTargetY = -nY;
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC59CB0
// 1.13c D2Game+0x41DE0
int32_t __fastcall MISSMODE_SrvDo32_TigerFury(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wSubMissile[0] < 0)
    {
        return 2;
    }

    if (D2_CALL(D2Common_10234, pMissile->pDynamicPath))
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 1;
        missileParams.pOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);
        missileParams.pOrigin = pMissile;
        missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
        missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[0];
        missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);
        D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
    }

    return D2_CALL(MISSMODE_SrvDo07_GuidedArrow_BoneSpirit, pGame, pMissile);
}

//D2Game.0x6FC59D80
// 1.13c D2Game+0x41C30 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo34_BaalTauntControl(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }
    
    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    // retail reads the remaining frames before the sub-missile count, and ends in the PoisonCloud routine
    const int32_t nRemainingFrames = D2_CALL(MISSILE_GetRemainingFrames, pMissile);

    int32_t nCounter = 0;
    do
    {
        if ((int16_t)pMissilesTxtRecord->wSubMissile[nCounter] < 0 || pMissilesTxtRecord->dwParam[nCounter + 1] <= 0)
        {
            break;
        }

        ++nCounter;
    }
    while (nCounter < 3);

    if (nCounter <= 0)
    {
        return 2;
    }

    if (nRemainingFrames >= pMissilesTxtRecord->dwParam[0])
    {
        D2_CALL(SEED_InitLowSeed, &pMissile->pSeed, D2_CALL(CLIENTS_GetUnitX, pMissile));

        const int32_t nIndex = D2_CALL(ITEMS_RollLimitedRandomNumber, &pMissile->pSeed, nCounter);
        if (!(nRemainingFrames % pMissilesTxtRecord->dwParam[nIndex + 1]))
        {
            D2MissileStrc missileParams = {};
            missileParams.pOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);
            missileParams.pOrigin = pMissile;
            missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
            missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);
            missileParams.nMissile = (int16_t)pMissilesTxtRecord->wSubMissile[nIndex];

            D2UnitStrc* pSubMissile = D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
            if (pSubMissile)
            {
                D2_CALL(MISSMODE_SrvDmgHitHandler, pGame, pSubMissile, nullptr, 1);
            }
        }
    }

    return D2_CALL(MISSMODE_SrvDo03_PoisonCloud_Blizzard_ThunderStorm_HandOfGod, pGame, pMissile);
}

//D2Game.0x6FC59F
// 1.13c D2Game+0x3F890
int32_t __fastcall MISSMODE_SrvDo35_RoyalStrikeChaosIce(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    int32_t nPeriod = pMissilesTxtRecord->dwParam[0];
    if (nPeriod < 1)
    {
        nPeriod = 1;
    }

    if (MISSILE_GetRemainingFrames(pMissile) % nPeriod != 0)
    {
        return MISSMODE_HandleMissileCollision(pGame, pMissile);
    }

    // The target X field carries the low seed between pulses, target Y the packed step.
    SEED_InitLowSeed(&pMissile->pSeed, MISSILE_GetTargetX(pMissile));

    const int32_t nPacked = MISSILE_GetTargetY(pMissile);
    const int32_t nStepX = (int16_t)nPacked;
    const int32_t nStepY = (int16_t)((uint32_t)nPacked >> 16);

    // Retail: an even roll negates the X step on the Y side, an odd roll negates the Y step on the X side.
    int32_t nTurnX = nStepY;
    int32_t nTurnY = -nStepX;
    if (SEED_RollRandomNumber(&pMissile->pSeed) & 1)
    {
        nTurnX = -nStepY;
        nTurnY = nStepX;
    }

    int32_t nX = (nTurnX + 4 * nStepX) / 4;
    int32_t nY = (nTurnY + 4 * nStepY) / 4;
    if (!nX)
    {
        nX = 1;
    }

    if (!nY)
    {
        nY = 1;
    }

    const int32_t nUnitY = CLIENTS_GetUnitY(pMissile);
    const int32_t nUnitX = CLIENTS_GetUnitX(pMissile);
    D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nUnitX + nX, nUnitY + nY);
    D2Common_10142(pMissile->pDynamicPath, pMissile, 0);

    MISSILE_SetTargetX(pMissile, SEED_GetLowSeed(&pMissile->pSeed));
    MISSILE_SetTargetY(pMissile, (nY << 16) + (nX & 0xFFFF));

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC5A180
// 1.13c D2Game+0x3B990
int32_t __fastcall MISSMODE_SrvHit01_Fireball_ExplodingArrow_FreezingArrowExplosion(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange < 1)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2DamageStrc damage = {};
    MISSMODE_GetDamageValue(pGame, pMissile, pUnit, &damage);
    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;
    // Retail inlines the aura sweep wrapper with the default filter.
    sub_6FD0FE80(pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);
    return 1;
}

//D2Game.0x6FC5A330
// 1.13c D2Game+0x3B7C0
int32_t __fastcall MISSMODE_SrvHit24_PantherPotOrange(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange < 1)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2DamageStrc damage = {};
    MISSMODE_FillDamageParams(pMissile, pUnit, &damage);
    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;
    // Retail inlines the aura sweep wrapper with the default filter.
    sub_6FD0FE80(pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);
    return 1;
}

//D2Game.0x6FC5A4F0
// 1.13c D2Game+0x3E330
int32_t __fastcall MISSMODE_SrvHit02_PlagueJavelin_PoisonPotion(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 3;
    }

    D2UnitStrc* pOrigin = pUnit ? pUnit : pMissile;
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    MISSMODE_CreatePoisonCloudHitSubmissiles(pGame, pOwner, pOrigin, pMissilesTxtRecord->wHitSubMissile[0], nSkillId, nLevel, pMissilesTxtRecord->dwHitPar[0], pMissilesTxtRecord->dwHitPar[1], pMissilesTxtRecord->dwHitPar[2]);
    return 3;
}

//D2Game.0x6FC5A580
// 1.13c D2Game+0x3B5F0
int32_t __fastcall MISSMODE_SrvHit44_ExplodingJavelin(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (!nRange)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2DamageStrc damage = {};
    MISSMODE_FillDamageParams(pMissile, pUnit, &damage);
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;
    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;

    // Retail inlines the aura sweep wrapper with the default filter; the hit always returns 1.
    if (pOwner)
    {
        sub_6FD0FE80(pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);
    }
    return 1;
}

//D2Game.0x6FC5A740
// 1.13c D2Game+0x3BB60
int32_t __fastcall MISSMODE_SrvHit03_ExplosivePotion_BombOnGround(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (pUnit)
    {
        return 0;
    }

    return MISSMODE_SrvHit44_ExplodingJavelin(pGame, pMissile, nullptr);
}

//D2Game.0x6FC5A760
// 1.13c D2Game+0x39FD0
int32_t __fastcall MISSMODE_SrvHit07_HolyBolt_FistOfTheHeavenBolt(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    if (!pUnit)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    // Retail inlines the owner lookup (owner flag, then E03A0); SUNIT_GetOwner is that same test.
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pMissilesTxtRecord->dwHitPar[0] || !pOwner || !SUNIT_CanPetBeTargetedBySkill(pGame, pOwner, pUnit, nSkillId) && !SUNIT_CanAllyBeTargetedBySkill(pGame, pOwner, pUnit, nSkillId))
    {
        if (pUnit->dwUnitType == UNIT_PLAYER)
        {
            if (!pMissilesTxtRecord->dwHitPar[1])
            {
                return 3;
            }
        }
        else if (pUnit->dwUnitType == UNIT_MONSTER)
        {
            if (!pMissilesTxtRecord->dwHitPar[1])
            {
                return 3;
            }

            if (pMissilesTxtRecord->dwHitPar[1] == 2)
            {
                if (MONSTERS_IsDemon(pUnit))
                {
                    return 3;
                }
            }
            else
            {
                if (MONSTERS_IsUndead(pUnit))
                {
                    return 3;
                }
            }
        }

        return 4;
    }

    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    const int32_t nMin = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel) << 8;
    const int32_t nMax = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[1], nSkillId, nLevel) << 8;
    int32_t nRand = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, nMax - nMin) + nMin;
    if (nRand < 1)
    {
        nRand = MISSMODE_RollDamageValue(pMissile, STAT_MINDAMAGE, STAT_MAXDAMAGE, -1);
    }

    const int32_t nHp = STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0);
    const int32_t nMaxHp = STATLIST_GetMaxLifeFromUnit(pUnit);
    int32_t nNewHp = nHp + nRand;
    if (nMaxHp <= nNewHp)
    {
        nNewHp = nMaxHp;
    }
    STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, nNewHp, 0);

    if (pMissilesTxtRecord->wProgOverlay > 0)
    {
        UNITS_SetOverlay(pUnit, pMissilesTxtRecord->wProgOverlay, 0);
    }

    return 1;
}

//D2Game.0x6FC5A9B0
// 1.13c D2Game+0x39F70
int32_t __fastcall MISSMODE_SrvHit08_Blaze(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pUnit || !pMissile)
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 2;
    }

    if (pUnit == pOwner && STATES_CheckState(pOwner, STATE_BLAZE))
    {
        return 0;
    }

    return 2;
}

//D2Game.0x6FC5AA00
// 1.13c D2Game+0x3BD10
int32_t __fastcall MISSMODE_SrvHit06_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nMonsterId = pMissilesTxtRecord->dwHitPar[0];
    if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount)
    {
        return 1;
    }

    int32_t nSpawnMode = MONMODE_NEUTRAL;
    if (pMissilesTxtRecord->dwHitPar[1] >= 0 && pMissilesTxtRecord->dwHitPar[1] < 16)
    {
        nSpawnMode = pMissilesTxtRecord->dwHitPar[1];
    }

    const int32_t nY = CLIENTS_GetUnitY(pMissile);
    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    D2GAME_SpawnPresetMonster_6FC66560(pGame, UNITS_GetRoom(pMissile), nMonsterId, nX, nY, nSpawnMode);
    return 1;
}

//D2Game.0x6FC5AB00
// 1.13c D2Game+0x3E0B0
int32_t __fastcall MISSMODE_SrvHit09_ImmolationArrow(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pMissilesTxtRecord->wHitSubMissile[0] >= 0)
    {
        STATLIST_SetUnitStat(pMissile, STAT_COLDLENGTH, 0, 0);
        STATLIST_SetUnitStat(pMissile, STAT_ITEM_FREEZE, 0, 0);

        int32_t nCount = pMissilesTxtRecord->dwHitPar[0];
        if (nCount < 1)
        {
            nCount = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
            if (nCount < 1)
            {
                nCount = 1;
            }
        }

        const int32_t nHitCalc = MISSILE_EvaluateMissileFormula(pMissile, pOwner, pMissilesTxtRecord->dwHitCalc, pMissile->dwClassId, nLevel);
        MISSMODE_CreateImmolationArrowHitSubmissiles(pGame, pMissile, nCount, pMissilesTxtRecord->wHitSubMissile[0], nHitCalc);
    }

    int32_t nRange = pMissilesTxtRecord->dwHitPar[1];
    if (nRange < 1)
    {
        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[1], nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    D2DamageStrc damage = {};
    MISSMODE_GetDamageValue(pGame, pMissile, pUnit, &damage);
    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;

    const int32_t nY = CLIENTS_GetUnitY(pMissile);
    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    D2UnitStrc* pAreaOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pAreaOwner)
    {
        sub_6FD10200(pGame, pAreaOwner, nX, nY, nRange, &damage, 0);
    }

    return 3;
}

// Prior art: D2MOO SrvHit10 flight reset; checked against the 1.13c helper.
// 1.13c D2Game+0x397C0
void __fastcall MISSMODE_ResetGuidedFlight(D2UnitStrc* pMissile, D2UnitStrc* pTarget)
{
    if (!pMissile)
    {
        return;
    }
    D2MissilesTxt* pRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pRecord)
    {
        return;
    }
    if (pTarget && SUNIT_IsDead(pTarget))
    {
        pTarget = nullptr;
    }
    // Retail sign-extends these table words before its 32-bit multiply/add.
    const int32_t nFrames = int32_t((uint32_t(MISSILE_GetLevel(pMissile)) - 1u)
        * uint32_t(int16_t(pRecord->wLevRange)) + uint32_t(int16_t(pRecord->wRange)));
    MISSILE_SetTotalFrames(pMissile, nFrames);
    MISSILE_SetCurrentFrame(pMissile, nFrames);
    int32_t nDistance;
    if (pTarget)
    {
        PATH_SetTargetUnit(pMissile->pDynamicPath, pTarget);
        MISSILE_SetTargetX(pMissile, 5);
        nDistance = UNITS_GetDistanceToOtherUnit(pMissile, pTarget);
    }
    else
    {
        const uint32_t nOffset = MISSILE_GetTargetY(pMissile);
        const int32_t nX = int32_t(uint32_t(CLIENTS_GetUnitX(pMissile)) + uint32_t(int16_t(nOffset)));
        const int32_t nY = int32_t(uint32_t(CLIENTS_GetUnitY(pMissile)) + uint32_t(int16_t(nOffset >> 16)));
        D2COMMON_10170_PathSetTargetPos(pMissile->pDynamicPath, nX, nY);
        MISSILE_SetTargetX(pMissile, 6);
        nDistance = UNITS_GetDistanceToCoordinates(pMissile, nX, nY);
    }
    if (nDistance < 25)
    {
        D2Common_10142(pMissile->pDynamicPath, pMissile, 0);
    }
}

// Retarget a guided missile whose flight ran out: pick a new target around it and restart the flight.
// 1.13c D2Game+0x3B070
static int32_t __fastcall MISSMODE_RetargetGuidedMissile(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nRange = pMissilesTxtRecord->dwParam[1];
    if (MISSILE_GetTargetX(pMissile) & 4)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nY = CLIENTS_GetUnitY(pMissile);
    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    D2UnitStrc* pTarget = sub_6FD107F0(pGame, pOwner, nX, nY, nRange, 3, -1, nullptr);
    MISSMODE_ResetGuidedFlight(pMissile, pTarget);
    return 0;
}

//D2Game.0x6FC5AD40
// 1.13c D2Game+0x3B140
int32_t __fastcall MISSMODE_SrvHit10_GuidedArrow_BoneSpirit(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (DUNGEON_IsRoomInTown(UNITS_GetRoom(pMissile)))
    {
        return 1;
    }

    const int32_t nFlags = MISSILE_GetTargetX(pMissile);
    if (!pUnit && nFlags & 4)
    {
        return 1;
    }

    const int32_t nPathFlags = D2Common_10201(pMissile->pDynamicPath);
    if (!pUnit && nPathFlags & 4)
    {
        return 1;
    }

    if (nFlags & 1)
    {
        if (pUnit)
        {
            sub_6FCBC7E0(pGame, pMissile);
            D2UnitStrc* pTarget = PATH_GetTargetUnit(pMissile->pDynamicPath);
            if (pTarget == pMissile)
            {
                pTarget = nullptr;
            }

            if (pUnit != pTarget)
            {
                return 4;
            }
        }

        return 3;
    }

    if (!(nFlags & 2))
    {
        return 3;
    }

    if (MISSILE_GetCurrentFrame(pMissile) >= 1)
    {
        return (nFlags & 4) ? 3 : 4;
    }

    return MISSMODE_RetargetGuidedMissile(pGame, pMissile) ? 1 : 4;
}

//D2Game.0x6FC5B020
// 1.13c D2Game+0x3D020
int32_t __fastcall MISSMODE_SrvHit12_ChainLightning_LightningStrike(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || !pUnit)
    {
        return 3;
    }

    const int32_t nParam = MISSILE_GetTargetX(pMissile);
    if (nParam <= 1)
    {
        return 3;
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange < 1)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 3;
        }

        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    D2UnitStrc* pTarget = sub_6FD107F0(pGame, pOwner, nX, nY, nRange, 0x88583u, pUnit->dwUnitId, 0);
    if (pTarget && pTarget != pUnit)
    {
        D2MissileStrc missileParams = {};
        missileParams.dwFlags = 0x21;
        missileParams.nMissile = pMissile->dwClassId;
        missileParams.pOwner = pOwner;
        missileParams.pTarget = pTarget;
        missileParams.nX = nX;
        missileParams.nY = nY;
        missileParams.nTargetX = CLIENTS_GetUnitX(pTarget);
        missileParams.nTargetY = CLIENTS_GetUnitY(pTarget);
        missileParams.nSkill = MISSILE_GetSkill(pMissile);
        missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);

        D2UnitStrc* pSubMissile = MISSILES_CreateMissileFromParams(pGame, &missileParams);
        if (pSubMissile)
        {
            MISSILE_SetTargetX(pSubMissile, nParam - 1);
        }
    }

    return 3;
}

// Put an element length into the damage block slot the missile's element uses.
// 1.13c D2Game+0x394B0
static void __fastcall MISSMODE_AssignElementLength(D2UnitStrc* pMissile, D2DamageStrc* pDamage, int32_t nLength)
{
    if (!pMissile)
    {
        return;
    }

    switch (MISSILE_GetElemTypeFromMissileId(pMissile->dwClassId))
    {
    case ELEMTYPE_COLD:
        pDamage->dwColdLen = nLength;
        return;

    case ELEMTYPE_POIS:
        pDamage->dwPoisLen = nLength;
        return;

    case ELEMTYPE_STUN:
        pDamage->dwStunLen = nLength;
        return;

    case ELEMTYPE_BURN:
        pDamage->dwBurnLen = nLength;
        return;

    case ELEMTYPE_FREEZE:
        pDamage->dwFrzLen = nLength;
        return;

    default:
        return;
    }
}

//D2Game.0x6FC5B280
// 1.13c D2Game+0x3B3E0
int32_t __fastcall MISSMODE_SrvHit13_GlacialSpike_HellMeteorDown(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange < 1)
    {
        nRange = SKILLS_EvaluateSkillFormula(SUNIT_GetOwner(pGame, pMissile), pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel);
        if (nRange < 1)
        {
            nRange = 1;
        }
    }

    int32_t nLength = pMissilesTxtRecord->dwHitPar[1];
    if (nLength < 1)
    {
        nLength = SKILLS_EvaluateSkillFormula(SUNIT_GetOwner(pGame, pMissile), pSkillsTxtRecord->dwAuraLenCalc, nSkillId, nLevel);
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2DamageStrc damage = {};
    MISSMODE_GetDamageValue(pGame, pMissile, pUnit, &damage);
    if (nLength > 0)
    {
        MISSMODE_AssignElementLength(pMissile, &damage, nLength);
    }

    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner)
    {
        sub_6FD10200(pGame, pOwner, nX, nY, nRange, &damage, 0);
    }

    return 1;
}

//D2Game.0x6FC5B4E0
// 1.13c D2Game+0x3F220 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit04_ExplodingArrow_FreezingArrow_RoyalStrikeMeteorCenter(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
    missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);

    for (int32_t i = 0; i < 4; ++i)
    {
        if ((int16_t)pMissilesTxtRecord->wHitSubMissile[i] > 0)
        {
            missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[i];

            D2UnitStrc* pHitSubMissile = D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
            if (pHitSubMissile && (int32_t)pMissilesTxtRecord->dwHitPar[0] > 0)   // 1.13c compares signed
            {
                D2_CALL(MISSMODE_SrvDmgHitHandler, pGame, pHitSubMissile, pUnit, 1);
            }
        }
    }

    return 3;
}

//D2Game.0x6FC5B5C0
// 1.13c D2Game+0x3F120
int32_t __fastcall MISSMODE_SrvHit11_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);

    for (int32_t i = 0; i < 4; ++i)
    {
        if (pMissilesTxtRecord->wHitSubMissile[i] > 0)
        {
            missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[i];
            D2UnitStrc* pHitSubMissile = MISSILES_CreateMissileFromParams(pGame, &missileParams);
            if (pHitSubMissile && pMissilesTxtRecord->dwHitPar[0] > 0)
            {
                MISSMODE_SrvDmgHitHandler(pGame, pHitSubMissile, pUnit, 1);
            }
        }
    }

    return 1;
}

//D2Game.0x6FC5B6A0
// 1.13c D2Game+0x1A40: the skill's Param[2] + (level - 1) * Param[3], 0 for a bad skill id or a level below 1
int32_t __fastcall MISSMODE_GetSkillRangeParam(int32_t nSkillId, int32_t nLevel)
{
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (pSkillsTxtRecord && nLevel > 0)
    {
        return pSkillsTxtRecord->dwParam[2] + (nLevel - 1) * pSkillsTxtRecord->dwParam[3];
    }
    return 0;
}

// 1.13c D2Game+0x3DEF0 (HitPar clamp signed: cmp/jge, was unsigned; X/Y read before the range; area damage direct, always returns 1)
int32_t __fastcall MISSMODE_SrvHit14_MeteorCenter_CatapultMeteor_RoyalStrikeMeteor(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
    const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pMissile);
    D2SkillsTxt* pSkillsTxtRecord = D2_CALL(SKILLS_GetSkillsTxtRecord, nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pMissile);
    const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pMissile);

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange <= 0)
    {
        nRange = std::max(D2_CALL(SKILLS_EvaluateSkillFormula, pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel), 1);
    }

    D2DamageStrc damage = {};
    D2_CALL(MISSMODE_FillDamageParams, pMissile, pUnit, &damage);

    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;

    D2_CALL(sub_6FD0FE80, pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);

    const int32_t nHitSubMissile = (int16_t)pMissilesTxtRecord->wHitSubMissile[0];
    if (nHitSubMissile >= 0)
    {
        const int32_t nMissileRange = D2_CALL(MISSMODE_GetSkillRangeParam, nSkillId, nLevel);
        const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[1]), 1);
        D2_CALL(MISSMODE_CreateMeteor_MoltenBoulderSubmissiles, pGame, pMissile, nHitSubMissile, nMissileRange, nParam);
    }

    return 1;
}

//D2Game.0x6FC5B910
// 1.13c D2Game+0x3CEC0 (checked against decompile, no delta)
void __fastcall MISSMODE_CreateMeteor_MoltenBoulderSubmissiles(D2GameStrc* pGame, D2UnitStrc* pMissile, int32_t nMissileId, int32_t nRange, int32_t nStep)
{
    if (nMissileId < 0)
    {
        return;
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 1;
    if (nRange > 0)
    {
        missileParams.dwFlags = 0x8001;
        missileParams.nRange = nRange;
    }

    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = nMissileId;

    nStep = std::max(nStep, 1);

    constexpr int32_t xOffsets[18] =
    {
        2, -2, 0, 0, -3, 0, 3, -1, 1, -1, 2, -4, -3, -1, 0, 1, 3, 4
    };

    constexpr int32_t yOffsets[18] =
    {
        -2, -2, 2, 5, 3, 3, 3, 2, 1, -1, -1, -2, -2, -3, -4, -3, -3, -2
    };

    for (int32_t i = 0; i < 18; i += nStep)
    {
        missileParams.nX = nX + xOffsets[i];
        missileParams.nY = nY + yOffsets[i];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }
}

//D2Game.0x6FC5BA50
// 1.13c D2Game+0x3CDD0
int32_t __fastcall MISSMODE_SrvHit15_SpiderGooLay(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 1;
    missileParams.pOwner = pOwner;
    missileParams.nX = CLIENTS_GetUnitX(pMissile);
    missileParams.nY = CLIENTS_GetUnitY(pMissile);
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    MISSILES_CreateMissileFromParams(pGame, &missileParams);
    return 0;
}

//D2Game.0x6FC5BB80
// 1.13c D2Game+0x3EC10 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit16_SpiderGoo_VinesTrail_VinesWither(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
    const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    if (!pUnit)
    {
        return 0;
    }

    if (pSkillsTxtRecord->wAuraTargetState < 0 || pSkillsTxtRecord->wAuraTargetState >= sgptDataTables->nStatesTxtRecordCount)
    {
        return 1;
    }

    const int32_t nLength = std::max(D2_CALL(SKILLS_EvaluateSkillFormula, pOwner, pSkillsTxtRecord->dwCalc[3], nSkillId, nLevel), 5);

    D2StatListStrc* pStatList = D2_CALL(STATLIST_GetStatListFromUnitAndState, pUnit, pSkillsTxtRecord->wAuraTargetState);
    if (!pStatList)
    {
        D2StatListStrc* pNewStatList = D2_CALL(STATLIST_AllocStatList, pGame->pMemoryPool, 2u, nLength + pGame->dwGameFrame, pOwner->dwUnitType, pOwner->dwUnitId);
        if (!pNewStatList)
        {
            return 1;
        }

        D2_CALL(STATLIST_SetState, pNewStatList, pSkillsTxtRecord->wAuraTargetState);
        D2_CALL(STATLIST_SetStatRemoveCallback, pNewStatList, sub_6FD10E50);
        D2_CALL(D2COMMON_10475_PostStatToStatList, pUnit, pNewStatList, 1);
        D2_CALL(STATES_ToggleState, pUnit, pSkillsTxtRecord->wAuraTargetState, 1);

        // TODO: pStatList = pNewStatList; ????
    }

    D2_CALL(sub_6FCFE0E0, pUnit, pStatList, pSkillsTxtRecord, nSkillId, nLevel);
    D2_CALL(STATES_ToggleGfxStateFlag, pUnit, pSkillsTxtRecord->wAuraTargetState, 1);
    D2_CALL(D2COMMON_10476, pStatList, nLength + pGame->dwGameFrame);
    D2_CALL(EVENT_SetEvent, pGame, pUnit, EVENTTYPE_REMOVESTATE, nLength + pGame->dwGameFrame, 0, 0);
    return 0;
}

//D2Game.0x6FC5BD30
// 1.13c D2Game+0x3EAE0
int32_t __fastcall MISSMODE_SrvHit17_Howl(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
    {
        return 0;
    }

    if (pSkillsTxtRecord->wAuraTargetState < 0 || pSkillsTxtRecord->wAuraTargetState >= sgptDataTables->nStatesTxtRecordCount)
    {
        return 1;
    }

    if (STATES_CheckState(pUnit, pSkillsTxtRecord->wAuraTargetState))
    {
        return 0;
    }

    // Retail reads the owner level, then Param2 (skill +0x14C), and compares against the target's level.
    const int32_t nOwnerLevel = STATLIST_UnitGetStatValue(pOwner, STAT_LEVEL, 0);
    const int32_t nLevelBonus = pSkillsTxtRecord->dwParam[1];
    if (nLevelBonus + nOwnerLevel + nLevel > (int32_t)STATLIST_UnitGetStatValue(pUnit, STAT_LEVEL, 0))
    {
        // Duration is Param5 + (level - 1) * Param6 (the 1.13c helper at 0x1A90); 1.10f's unused Param3/4 term is gone.
        int32_t nDuration = 0;
        if (nLevel > 0)
        {
            nDuration = pSkillsTxtRecord->dwParam[4] + (nLevel - 1) * pSkillsTxtRecord->dwParam[5];
        }

        AIUTIL_ApplyTerrorCurseState(pGame, pOwner, pUnit, nSkillId, 0, nDuration);
    }

    return 0;
}

//D2Game.0x6FC5BED0
// 1.13c D2Game+0x3EA60
int32_t __fastcall MISSMODE_SrvHit18_Shout_BattleCommand_BattleOrders(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    if (pUnit && SUNIT_AreUnitsAligned(pGame, pOwner, pUnit))
    {
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        SKILLS_ApplyWarcryStats(pGame, pUnit, pOwner, MISSILE_GetSkill(pMissile), nLevel);
    }

    return 0;
}

//D2Game.0x6FC5BF30
// 1.13c D2Game+0x3E8A0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit19_FingerMageSpider(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || !pUnit || pSkillsTxtRecord->wAuraTargetState < 0 || pSkillsTxtRecord->wAuraTargetState >= sgptDataTables->nStatesTxtRecordCount)
    {
        return 1;
    }

    const int32_t nLength = std::max(SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraLenCalc, nSkillId, nLevel), 5);
    const int32_t nTimeout = nLength + pGame->dwGameFrame;
    D2StatListStrc* pStatList = STATLIST_GetStatListFromUnitAndState(pUnit, pSkillsTxtRecord->wAuraTargetState);
    if (!pStatList)
    {
        D2StatListStrc* pNewStatList = STATLIST_AllocStatList(pGame->pMemoryPool, 2u, nTimeout, pOwner->dwUnitType, pOwner->dwUnitId);
        if (!pNewStatList)
        {
            return 1;
        }

        STATLIST_SetState(pNewStatList, pSkillsTxtRecord->wAuraTargetState);
        STATLIST_SetStatRemoveCallback(pNewStatList, sub_6FD10E50);
        D2COMMON_10475_PostStatToStatList(pUnit, pNewStatList, 1);
        STATES_ToggleState(pUnit, pSkillsTxtRecord->wAuraTargetState, 1);

        // TODO: pStatList = pNewStatList; ????
    }

    sub_6FCFE0E0(pUnit, pStatList, pSkillsTxtRecord, nSkillId, nLevel);
    STATES_ToggleGfxStateFlag(pUnit, pSkillsTxtRecord->wAuraTargetState, 1);
    D2COMMON_10476(pStatList, nTimeout);
    EVENT_SetEvent(pGame, pUnit, EVENTTYPE_REMOVESTATE, nTimeout, 0, 0);
    return 3;
}

//D2Game.0x6FC5C0D0
// 1.13c D2Game+0x3DD60 (X/Y read before the HitPar formulas)
int32_t __fastcall MISSMODE_SrvHit20_LightningFury(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
    const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pMissile);

    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = D2_CALL(SUNIT_GetOwner, pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    // retail reads the missile position before evaluating the two formulas
    const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pMissile);
    const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pMissile);

    int32_t nAuraRange = pMissilesTxtRecord->dwHitPar[0];
    if (nAuraRange <= 0)
    {
        nAuraRange = std::max(D2_CALL(SKILLS_EvaluateSkillFormula, pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel), 1);
    }

    int32_t nParam = pMissilesTxtRecord->dwHitPar[1];
    if (nParam <= 0)
    {
        nParam = std::max(D2_CALL(SKILLS_EvaluateSkillFormula, pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel), 1);
    }

    int32_t nAuraFilter = pSkillsTxtRecord->dwAuraFilter;
    if (!nAuraFilter)
    {
        nAuraFilter = 0xA783u;
    }

    D2HitFunc22ParamStrc params = {};
    params.pOwner = pOwner;
    params.pOrigin = pMissile;
    params.nSkillId = nSkillId;
    params.nLevel = nLevel;
    params.nParam = nParam;
    params.nHitSubMissile = pMissilesTxtRecord->wHitSubMissile[0];

    D2_CALL(sub_6FD0FE80, pGame, pOwner, nX, nY, nAuraRange, nAuraFilter, MISSMODE_LightningFury_AuraCallback, &params, 1, __FILE__, __LINE__);
    return 3;
}

//D2Game.0x6FC5C2D0
// 1.13c D2Game+0x3CD00 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_LightningFury_AuraCallback(D2AuraCallbackStrc* pAuraCallback, D2UnitStrc* pTarget)
{
    D2HitFunc22ParamStrc* pParams = (D2HitFunc22ParamStrc*)pAuraCallback->pArgs;
    if (pAuraCallback->nCounter >= pParams->nParam)
    {
        return 0;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 0x20;
    missileParams.pOwner = pParams->pOwner;
    missileParams.pOrigin = pParams->pOrigin;
    missileParams.nSkill = pParams->nSkillId;
    missileParams.nSkillLevel = pParams->nLevel;
    missileParams.nMissile = pParams->nHitSubMissile;
    missileParams.nTargetX = D2_CALL(CLIENTS_GetUnitX, pTarget);
    missileParams.nTargetY = D2_CALL(CLIENTS_GetUnitY, pTarget);
    D2_CALL(MISSILES_CreateMissileFromParams, pAuraCallback->pGame, &missileParams);

    return 1;
}

//D2Game.0x6FC5C3A0
// 1.13c D2Game+0x3ABD0
int32_t __fastcall MISSMODE_SrvHit21_BattleCry(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || !pUnit)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);

    // Retail bounds the state with jg: a state equal to the states count still passes.
    if (!pSkillsTxtRecord || pSkillsTxtRecord->wAuraTargetState < 0 || pSkillsTxtRecord->wAuraTargetState > sgptDataTables->nStatesTxtRecordCount)
    {
        return 1;
    }

    // Retail: a non-zero AuraFilter becomes 0xA783, a zero one stays zero.
    int32_t nAuraFilter = pSkillsTxtRecord->dwAuraFilter;
    if (nAuraFilter)
    {
        nAuraFilter = 0xA783;
    }

    if (!sub_6FD0FA00(pOwner, pUnit, nAuraFilter))
    {
        return 1;
    }

    D2CurseStrc curse = {};

    curse.pUnit = pOwner;
    curse.pTarget = pUnit;
    curse.nSkill = nSkillId;
    curse.nSkillLevel = nLevel;
    curse.nDuration = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraLenCalc, nSkillId, nLevel);
    curse.nState = pSkillsTxtRecord->wAuraTargetState;
    curse.pStateRemoveCallback = nullptr;

    D2StatListStrc* pStatList = sub_6FD10EC0(&curse);
    if (pStatList)
    {
        sub_6FCFE0E0(pUnit, pStatList, pSkillsTxtRecord, nSkillId, nLevel);
    }

    return 0;
}

//D2Game.0x6FC5C4D0
// 1.13c D2Game+0x3DB30
int32_t __fastcall MISSMODE_SrvHit22_FistOfTheHeavensDelay(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pTarget = SUNIT_GetServerUnit(pGame, MISSILE_GetTargetX(pMissile), MISSILE_GetTargetY(pMissile));
    if (!pTarget)
    {
        return 0;
    }

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    int32_t nAuraRange = pMissilesTxtRecord->dwHitPar[0];
    if (nAuraRange < 1)
    {
        nAuraRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel);
        if (nAuraRange < 1)
        {
            nAuraRange = 1;
        }
    }

    int32_t nParam = pMissilesTxtRecord->dwHitPar[1];
    if (nParam < 1)
    {
        nParam = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[3], nSkillId, nLevel);
        if (nParam < 1)
        {
            nParam = 1;
        }
    }

    D2DamageStrc damage = {};
    MISSMODE_FillDamageParams(pMissile, pTarget, &damage);
    // retail 3AFF0: the damage flags, then the target-AC reduction on non-hireling monsters
    MISSMODE_SetDamageFlags(pGame, pMissile, pTarget, &damage);

    if (pTarget->dwUnitType == UNIT_MONSTER && !MONSTERS_GetHirelingTypeId(pTarget))
    {
        const int32_t nDamageTargetAc = STATLIST_UnitGetStatValue(pMissile, STAT_ITEM_DAMAGETARGETAC, 0);
        if (nDamageTargetAc)
        {
            const int32_t nArmorClass = std::max(nDamageTargetAc + STATLIST_UnitGetStatValue(pTarget, STAT_ARMORCLASS, 0), 0);
            STATLIST_SetUnitStat(pTarget, STAT_ARMORCLASS, nArmorClass, 0);
        }
    }

    int32_t nAuraFilter = pSkillsTxtRecord->dwAuraFilter;
    if (!nAuraFilter)
    {
        nAuraFilter = 0xA683u;
    }

    D2HitFunc22ParamStrc params = {};
    params.pOwner = pOwner;
    params.pOrigin = pMissile;
    params.nSkillId = nSkillId;
    params.nLevel = nLevel;
    params.nParam = nParam;
    params.nHitSubMissile = pMissilesTxtRecord->wHitSubMissile[0];
    sub_6FD0FE80(pGame, pOwner, nX, nY, nAuraRange, nAuraFilter, MISSMODE_FistOfTheHeavensDelay_AuraCallback, &params, 0, __FILE__, __LINE__);
    return 1;
}

//D2Game.0x6FC5C790
// 1.13c D2Game+0x3CC30 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_FistOfTheHeavensDelay_AuraCallback(D2AuraCallbackStrc* pAuraCallback, D2UnitStrc* pTarget)
{
    D2HitFunc22ParamStrc* pParams = (D2HitFunc22ParamStrc*)pAuraCallback->pArgs;
    if (pParams->nParam > 0 && pAuraCallback->nCounter >= pParams->nParam)
    {
        return 0;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 0x20;
    missileParams.pOwner = pParams->pOwner;
    missileParams.pOrigin = pParams->pOrigin;
    missileParams.nSkill = pParams->nSkillId;
    missileParams.nSkillLevel = pParams->nLevel;
    missileParams.nMissile = pParams->nHitSubMissile;
    missileParams.nTargetX = D2_CALL(CLIENTS_GetUnitX, pTarget);
    missileParams.nTargetY = D2_CALL(CLIENTS_GetUnitY, pTarget);
    D2_CALL(MISSILES_CreateMissileFromParams, pAuraCallback->pGame, &missileParams);

    return 1;
}

//D2Game.0x6FC5C860
// 1.13c D2Game+0x3CB00 (diffed against the disassembly: same fields, same offset tables, no delta)
int32_t __fastcall MISSMODE_CreatePantherPotGreenSubmissiles(D2GameStrc* pGame, D2UnitStrc* pOwner, D2UnitStrc* pOrigin, int32_t nMissileId, int32_t nSkillId, int32_t nSkillLevel, int32_t nStep)
{
    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(nMissileId);
    if (!pMissilesTxtRecord)
    {
        return 0;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 0x1F;
    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pOrigin;
    missileParams.nMissile = nMissileId;
    missileParams.nX = CLIENTS_GetUnitX(pOrigin);
    missileParams.nY = CLIENTS_GetUnitY(pOrigin);
    missileParams.nSkill = nSkillId;
    missileParams.nSkillLevel = nSkillLevel;
    missileParams.nLoops = nSkillLevel - 1;
    missileParams.nVelocity = pMissilesTxtRecord->dwParam[0] << 7;

    constexpr int32_t xOffsets[] =
    {
        0, 2, 2, 2, 0, -2, -2, -2
    };

    constexpr int32_t yOffsets[] =
    {
        2, 2, 0, -2, -2, -2, 0, 2
    };

    for (int32_t i = 0; i < 8; i += nStep)
    {
        missileParams.nTargetX = xOffsets[i];
        missileParams.nTargetY = yOffsets[i];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return 1;
}

//D2Game.0x6FC5C990
// 1.13c D2Game+0x3DA60 (HitPar clamp signed: cmp/jge, was unsigned)
int32_t __fastcall MISSMODE_SrvHit25_PantherPotGreen(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    D2UnitStrc* pOrigin = pUnit;
    if (!pUnit)
    {
        pOrigin = pMissile;
    }

    const int32_t nHitPar = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);
    // retail asks for the level before the skill
    const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pMissile);
    const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
    D2_CALL(MISSMODE_CreatePantherPotGreenSubmissiles, pGame, pOwner, pOrigin, pMissilesTxtRecord->wHitSubMissile[0], nSkillId, nLevel, nHitPar);
    return 3;
}

//D2Game.0x6FC5CA50
// 1.13c D2Game+0x3C990
int32_t __fastcall MISSMODE_SrvHit26_GrimWardStart(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord)
    {
        return 1;
    }

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange < 1)
    {
        nRange = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[0], nSkillId, nLevel);
        if (nRange < 5)
        {
            nRange = 5;
        }
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 0x8001;
    missileParams.pOwner = pOwner;
    missileParams.nRange = nRange;
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    missileParams.nSkill = nSkillId;
    missileParams.nSkillLevel = nLevel;
    missileParams.nX = CLIENTS_GetUnitX(pMissile);
    missileParams.nY = CLIENTS_GetUnitY(pMissile);
    MISSILES_CreateMissileFromParams(pGame, &missileParams);
    return 1;
}

//D2Game.0x6FC5CBE0
// 1.13c D2Game+0x38EC0 (checked: retail returns 1, no delta)
int32_t __fastcall MISSMODE_SrvHit27_GrimWard(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    return 1;
}

//D2Game.0x6FC5CBF0
// 1.13c D2Game+0x3E740
int32_t __fastcall MISSMODE_SrvHit28_GrimWardScare(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
    if (!pSkillsTxtRecord || !pMissile || !SUNIT_GetOwner(pGame, pMissile) || !pUnit || pUnit->dwUnitType != UNIT_MONSTER)
    {
        return 1;
    }

    D2UnitStrc* pSubMissile = SUNIT_GetServerUnit(pGame, UNIT_MISSILE, MISSILE_GetTargetX(pMissile));
    if (!pSubMissile)
    {
        return 1;
    }

    // retail 21B10: Param1 + (lvl-1)*Param2, 0 below level 1
    const int32_t nDistance = nLevel > 0 ? pSkillsTxtRecord->dwParam[0] + (nLevel - 1) * pSkillsTxtRecord->dwParam[1] : 0;
    if (PATH_ComputeSquaredDistance(CLIENTS_GetUnitX(pSubMissile), CLIENTS_GetUnitY(pSubMissile), CLIENTS_GetUnitX(pUnit), CLIENTS_GetUnitY(pUnit)) < nDistance * nDistance)
    {
        // retail B2120 takes the duration only (skill Param6)
        AIUTIL_ApplyTerrorCurseState(pGame, pSubMissile, pUnit, nSkillId, 0, pSkillsTxtRecord->dwParam[5]);
    }
    return 1;
}

//D2Game.0x6FC5CE10
// 1.13c D2Game+0x3C850 (checked against decompile; nParam clamp is signed, was unsigned)
int32_t __fastcall MISSMODE_SrvHit29_FrozenOrb(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    if (MISSILE_GetCurrentFrame(pMissile))
    {
        return 2;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 2;
    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];

    const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);

    const int32_t xOffsets[64] =
    {
        30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2, 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29,
    };

    const int32_t yOffsets[64] =
    {
        0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2,
    };

    for (int32_t i = 0; i < 64; i += nParam)
    {
        missileParams.nTargetX = xOffsets[i];
        missileParams.nTargetY = yOffsets[i];

        D2UnitStrc* pHitSubMissile = MISSILES_CreateMissileFromParams(pGame, &missileParams);
        if (pHitSubMissile)
        {
            MISSILE_SetTargetX(pHitSubMissile, xOffsets[i]);
            MISSILE_SetTargetY(pHitSubMissile, yOffsets[i]);
        }
    }

    return 3;
}

const int32_t* gMissModeSrvHit05Tables[2] = {};

// 1.13c D2Game+0x3C700 (0x6FC5CF50 in 1.10f); nParam clamp is signed (cmp/jge) -- checked against decompile
int32_t __fastcall MISSMODE_SrvHit05_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    // 1.13c: requires the missile owner-info flag
    if (!(pMissile->dwFlagEx & UNITFLAGEX_STOREOWNERINFO))
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 2;
    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
    missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];

    const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);

    const int32_t xOffsets[64] =
    {
        30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2, 0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29
    };

    const int32_t yOffsets[64] =
    {
        0, 2, 5, 8, 11, 14, 16, 19, 21, 23, 24, 26, 27, 28, 29, 29, 30, 29, 29, 28, 27, 26, 24, 23, 21, 19, 16, 14, 11, 8, 5, 2, 0, -2, -5, -8, -11, -14, -16, -19, -21, -23, -24, -26, -27, -28, -29, -29, -30, -29, -29, -28, -27, -26, -24, -23, -21, -19, -16, -14, -11, -8, -5, -2
    };

    // the proof harness may point the two tables at the retail image's copies
    const int32_t* pXs = gMissModeSrvHit05Tables[0] ? gMissModeSrvHit05Tables[0] : xOffsets;
    const int32_t* pYs = gMissModeSrvHit05Tables[1] ? gMissModeSrvHit05Tables[1] : yOffsets;
    for (int32_t i = 0; i < 64; i += nParam)
    {
        missileParams.nTargetX = pXs[i];
        missileParams.nTargetY = pYs[i];

        D2UnitStrc* pHitSubMissile = D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
        if (pHitSubMissile)
        {
            D2_CALL(MISSILE_SetTargetX, pHitSubMissile, pXs[i]);
            D2_CALL(MISSILE_SetTargetY, pHitSubMissile, pYs[i]);
        }
    }

    return 3;
}

//D2Game.0x6FC5D070
// 1.13c D2Game+0x3A920
int32_t __fastcall MISSMODE_SrvHit31_FireHead(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner || !pUnit)
    {
        return 1;
    }

    D2DamageStrc damage = {};
    int32_t nHeal = MISSMODE_GetDamageValue(pGame, pMissile, pUnit, &damage);
    if (nHeal < 0)
    {
        nHeal = 0;
    }

    const int32_t nMaxHp = STATLIST_GetMaxLifeFromUnit(pOwner);
    int32_t nNewHp = (int32_t)STATLIST_UnitGetStatValue(pOwner, STAT_HITPOINTS, 0) + nHeal;
    if (nNewHp >= nMaxHp)
    {
        nNewHp = nMaxHp;
    }
    STATLIST_SetUnitStat(pOwner, STAT_HITPOINTS, nNewHp, 0);

    return (pMissilesTxtRecord->nCollideKill != 0) + 2;
}

//D2Game.0x6FC5D160
// 1.13c D2Game+0x3BE70 (the portal block is retail's 3BDA0, inlined here)
int32_t __fastcall MISSMODE_SrvHit32_CairnStones(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pUnit)
    {
        return 0;
    }

    if (!MISSILE_GetTargetX(pMissile))
    {
        MISSILE_SetTargetX(pMissile, 1);
        D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
        const int32_t nY = CLIENTS_GetUnitY(pMissile);
        const int32_t nX = CLIENTS_GetUnitX(pMissile);
        D2GAME_CreatePortalObject_6FD13DF0(pGame, pOwner, UNITS_GetRoom(pMissile), nX, nY, pMissilesTxtRecord->dwParam[3], 0, 60u, 1);
        DUNGEON_ToggleHasPortalFlag(UNITS_GetRoom(pMissile), 1);
    }

    return 0;
}

//D2Game.0x6FC5D270
// 1.13c D2Game+0x38EA0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit33_TowerChestSpawner(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pUnit)
    {
        D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);
        if (pRoom)
        {
            DUNGEON_ToggleHasPortalFlag(pRoom, 1);
        }
    }

    return 0;
}

//D2Game.0x6FC5D290
// 1.13c D2Game+0x3E670 (ENDANIM is FrameCnt[1] >> 8 ticks out, no +1)
int32_t __fastcall MISSMODE_SrvHit35_OrbMist(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (pUnit)
    {
        return 0;
    }

    D2UnitStrc* pObject = SUNIT_GetServerUnit(pGame, UNIT_OBJECT, MISSILE_GetTargetX(pMissile));
    if (!pObject)
    {
        return 0;
    }

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pObject);
    if (pRoom)
    {
        DUNGEON_ToggleHasPortalFlag(pRoom, 1);
    }

    if (pObject->dwAnimMode == 0)
    {
        UNITS_ChangeAnimMode(pObject, 1);
        D2ObjectsTxt* pObjectsTxtRecord = DATATBLS_GetObjectsTxtRecord(pObject->dwClassId);
        if (pObjectsTxtRecord)
        {
            EVENT_SetEvent(pGame, pObject, EVENTTYPE_ENDANIM, (pObjectsTxtRecord->dwFrameCnt[1] >> 8) + pGame->dwGameFrame, 0, 0);
        }
    }

    return 1;
}

//D2Game.0x6FC5D320
// 1.13c D2Game+0x3AB30
int32_t __fastcall MISSMODE_SrvHit36_MissileInAir(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    if (pUnit)
    {
        return 0;
    }

    const int32_t nY = CLIENTS_GetUnitY(pMissile);
    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    D2UnitStrc* pHitSubMissile = D2GAME_CreateMissile_6FD115E0(pGame, pOwner, nSkillId, nLevel, pMissilesTxtRecord->wHitSubMissile[0], nX, nY);
    if (pHitSubMissile)
    {
        MISSILE_SetTargetX(pHitSubMissile, MISSILE_GetTargetX(pMissile));
    }
    return 1;
}

//D2Game.0x6FC5D430
// 1.13c D2Game+0x38E90 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit37_BladeCreeper(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (pUnit)
    {
        return 2;
    }

    return 0;
}

//D2Game.0x6FC5D440
// 1.13c D2Game+0x39210 (checked against decompile, no delta)
void __fastcall MISSMODE_CatapultChargedBall_LightningTrailingJavelin_SubmissileInitFunc(D2UnitStrc* pMissile, int32_t nUnused)
{
    if (!pMissile)
    {
        return;
    }

    const int32_t nRange = std::min(MISSILE_GetTotalFrames(pMissile), 255);

    SEED_InitLowSeed(&pMissile->pSeed, D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath));
    PATH_SetType(pMissile->pDynamicPath, PATHTYPE_CHARGEDBOLT);
    PATH_SetNewDistance(pMissile->pDynamicPath, nRange);
    D2Common_10142(pMissile->pDynamicPath, pMissile, 0);
}

//D2Game.0x6FC5D490
// 1.13c D2Game+0x3C5B0
int32_t __fastcall MISSMODE_SrvHit38_CatapultChargedBall(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 3;
    missileParams.nX = CLIENTS_GetUnitX(pMissile);
    missileParams.nY = CLIENTS_GetUnitY(pMissile);
    missileParams.pOwner = pOwner;
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    missileParams.nSkill = nSkillId;
    missileParams.nSkillLevel = nLevel;
    missileParams.pInitFunc = MISSMODE_CatapultChargedBall_LightningTrailingJavelin_SubmissileInitFunc;

    int32_t nCalc = (MISSILE_GetLevel(pMissile) - 1) * (int32_t)pMissilesTxtRecord->dwHitPar[1] + (int32_t)pMissilesTxtRecord->dwHitPar[0];
    if (nCalc < 1)
    {
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nCalc = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[3], nSkillId, nLevel);
        if (nCalc < 1)
        {
            nCalc = 1;
        }
    }

    sub_6FD14260(pGame, nCalc, &missileParams);
    return 1;
}

//D2Game.0x6FC5D620
// 1.13c D2Game+0x3BC00 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit39_ImpSpawnMonsters(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    const int32_t nY = CLIENTS_GetUnitY(pMissile);
    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    MONSTERSPAWN_SpawnRandomMonsterForLevel(pGame, UNITS_GetRoom(pMissile), nX, nY);
    return 1;
}

//D2Game.0x6FC5D6A0
// 1.13c D2Game+0x3C490
int32_t __fastcall MISSMODE_SrvHit40_CatapultSpikeBall(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    int32_t nCalc = (nLevel - 1) * (int32_t)pMissilesTxtRecord->dwHitPar[1] + (int32_t)pMissilesTxtRecord->dwHitPar[0];
    if (nCalc < 1)
    {
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(MISSILE_GetSkill(pMissile));
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nCalc = SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwCalc[3], MISSILE_GetSkill(pMissile), nLevel);
        if (nCalc < 1)
        {
            nCalc = 1;
        }
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    pOwner = SUNIT_GetOwner(pGame, pMissile);
    sub_6FCF8330(pGame, pMissile, pOwner, pMissilesTxtRecord->wHitSubMissile[0], nCalc, nCalc / 4, nSkillId, nLevel);
    return 1;
}

//D2Game.0x6FC5D7C0
// 1.13c D2Game+0x39E30
int32_t __fastcall MISSMODE_SrvHit43_HealingVortex(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || !pUnit)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);

    if (!SKILLS_GetSkillsTxtRecord(nSkillId))
    {
        return 1;
    }

    const int32_t nMinDamage = SKILLS_GetMinPhysDamage(SUNIT_GetOwner(pGame, pMissile), nSkillId, nLevel, 1);
    const int32_t nMaxDamage = SKILLS_GetMaxPhysDamage(SUNIT_GetOwner(pGame, pMissile), nSkillId, nLevel, 1);
    const int32_t nDamage = ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, nMaxDamage - nMinDamage) + nMinDamage;

    const int32_t nHitpoints = STATLIST_UnitGetStatValue(pUnit, STAT_HITPOINTS, 0);
    const int32_t nMaxHp = STATLIST_GetMaxLifeFromUnit(pUnit);

    if (nHitpoints != nMaxHp && pMissilesTxtRecord->wProgOverlay > 0)
    {
        UNITS_SetOverlay(pUnit, pMissilesTxtRecord->wProgOverlay, 0);
    }

    int32_t nNewHp = nHitpoints + nDamage;
    if (nNewHp >= nMaxHp)
    {
        nNewHp = nMaxHp;
    }
    STATLIST_SetUnitStat(pUnit, STAT_HITPOINTS, nNewHp, 0);

    return pMissilesTxtRecord->nCollideKill != 0;
}

//D2Game.0x6FC5D950
// 1.13c D2Game+0x3C3B0 (the HitPar[0] count test is signed: a negative count returns 1, was unsigned)
int32_t __fastcall MISSMODE_SrvHit45_LightningTrailingJavelin(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = pMissile ? SKILLS_GetMissilesTxtRecord(pMissile->dwClassId) : nullptr;
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0 || (int32_t)pMissilesTxtRecord->dwHitPar[0] < 1)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 3;
    missileParams.nX = CLIENTS_GetUnitX(pMissile);
    missileParams.nY = CLIENTS_GetUnitY(pMissile);
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);

    if (pMissilesTxtRecord->dwHitPar[1])
    {
        missileParams.pInitFunc = MISSMODE_CatapultChargedBall_LightningTrailingJavelin_SubmissileInitFunc;
    }

    sub_6FD14260(pGame, pMissilesTxtRecord->dwHitPar[0], &missileParams);
    return 1;
}

//D2Game.0x6FC5DA80
// 1.13c D2Game+0x3D8C0 (HitPar clamp signed: cmp/jge, was unsigned; a null hit unit passes, the area damage is called directly, returns 1)
int32_t __fastcall MISSMODE_SrvHit47_MoltenBoulder(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    // retail: a hit unit is only required to be a large monster when one is given; none passes
    if (pUnit)
    {
        if (pUnit->dwUnitType != UNIT_MONSTER)
        {
            return 2;
        }

        if (!D2_CALL(MONSTERREGION_CheckMonStats2Flag, pUnit->dwClassId, MONSTATS2FLAGINDEX_LARGE))
        {
            return 2;
        }
    }

    const int32_t nX = D2_CALL(CLIENTS_GetUnitX, pMissile);
    const int32_t nY = D2_CALL(CLIENTS_GetUnitY, pMissile);

    D2DamageStrc damage = {};
    D2_CALL(MISSMODE_FillDamageParams, pMissile, pUnit, &damage);

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange <= 0)
    {
        const int32_t nSkillId = D2_CALL(MISSILE_GetSkill, pMissile);
        const int32_t nLevel = D2_CALL(MISSILE_GetLevel, pMissile);
        D2SkillsTxt* pSkillsTxtRecord = D2_CALL(SKILLS_GetSkillsTxtRecord, nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nRange = std::max(D2_CALL(SKILLS_EvaluateSkillFormula, pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel), 1);
    }

    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;

    // retail inlines the area-damage wrapper (0xA1240): filter 0x8583, the result is not used
    D2_CALL(sub_6FD0FE80, pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);

    const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[1]), 1);
    D2_CALL(MISSMODE_CreateMeteor_MoltenBoulderSubmissiles, pGame, pMissile, (int16_t)pMissilesTxtRecord->wHitSubMissile[0], nRange, nParam);
    return 1;
}

//D2Game.0x6FC5DD10
// 1.13c D2Game+0x3C260 (HitSubMissile test is a signed short: movsx/test/jl, was unsigned and never taken)
int32_t __fastcall MISSMODE_SrvHit48_MoltenBoulderEmerge(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 33;
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.nX = CLIENTS_GetUnitX(pMissile);
    missileParams.nY = CLIENTS_GetUnitY(pMissile);
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    missileParams.nTargetX = D2COMMON_10175_PathGetFirstPointX(pMissile->pDynamicPath);
    missileParams.nTargetY = D2COMMON_10176_PathGetFirstPointY(pMissile->pDynamicPath);
    MISSILES_CreateMissileFromParams(pGame, &missileParams);
    return 1;
}

//D2Game.0x6FC5DE50
// 1.13c D2Game+0x391A0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit50_PlagueVinesTrail(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    if (pUnit && (MISSILE_GetRemainingFrames(pMissile) < (int32_t)(MISSILE_GetTotalFrames(pMissile) - pMissilesTxtRecord->dwHitPar[0])))
    {
        return 2;
    }

    return 0;
}

//D2Game.0x6FC5DED0
// 1.13c D2Game+0x3C160 (HitSubMissile tests are signed shorts; unsigned they always created [1] and [2])
int32_t __fastcall MISSMODE_SrvHit51_VolcanoDebris(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    if ((int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.pOwner = SUNIT_GetOwner(pGame, pMissile);
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];
    missileParams.pOrigin = pMissile;
    MISSILES_CreateMissileFromParams(pGame, &missileParams);

    if ((int16_t)pMissilesTxtRecord->wHitSubMissile[1] >= 0)
    {
        missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[1];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    if ((int16_t)pMissilesTxtRecord->wHitSubMissile[2] >= 0)
    {
        missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[2];
        MISSILES_CreateMissileFromParams(pGame, &missileParams);
    }

    return 1;
}

//D2Game.0x6FC5DFC0
// 1.13c D2Game+0x3C030 (HitPar clamp signed: cmp/jge, was unsigned)
int32_t __fastcall MISSMODE_SrvHit52_BladeFury(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || (int16_t)pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 2;
    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = D2_CALL(MISSILE_GetSkill, pMissile);
    missileParams.nSkillLevel = D2_CALL(MISSILE_GetLevel, pMissile);

    const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);

    const int32_t xOffsets[] =
    {
        16, 16, 0, -16, -16, -16, 0, 16
    };

    const int32_t yOffsets[] =
    {
        0, 16, 16, 16, 0, -16, -16, -16
    };

    // 1.13c steps a 32-bit byte offset (step * 4), so a large step wraps like the shipped loop
    for (uint32_t off = 0; (int32_t)off < 0x20; off += (uint32_t)nParam * 4)
    {
        const uint32_t i = off / 4;
        missileParams.nTargetX = xOffsets[i];
        missileParams.nTargetY = yOffsets[i];

        D2UnitStrc* pHitSubMissile = D2_CALL(MISSILES_CreateMissileFromParams, pGame, &missileParams);
        if (pHitSubMissile)
        {
            D2_CALL(MISSILE_SetTargetX, pHitSubMissile, xOffsets[i]);
            D2_CALL(MISSILE_SetTargetY, pHitSubMissile, yOffsets[i]);
        }
    }

    return 1;
}

//D2Game.0x6FC5E0D0
// 1.13c D2Game+0x3F7D0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit53_RabiesContagion(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner && pUnit)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        const int32_t nFrame = MISSILE_GetTargetX(pMissile) - pGame->dwGameFrame;

        if (nFrame >= 10 && nFrame <= SKILLS_GetElementalLength(pOwner, nSkillId, nLevel, 1))
        {
            sub_6FCFEDD0(pGame, pOwner, pUnit, nFrame, nSkillId, nLevel);
            return 2;
        }
    }

    return 1;
}

//D2Game.0x6FC5E160
// 1.13c D2Game+0x3BC80 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit54_BaalSpawnMonsters(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pUnit)
    {
        D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
        if (pOwner)
        {
            D2SkillStrc* pSkill = SKILLS_GetHighestLevelSkillFromUnitAndId(pOwner, MISSILE_GetSkill(pMissile));
            if (pSkill)
            {
                D2GAME_SpawnPresetMonster_6FC66560(pGame, UNITS_GetRoom(pMissile), SKILLS_GetParam1(pSkill), CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), MONMODE_NEUTRAL);
            }
        }
    }

    return 1;
}

//D2Game.0x6FC5E220
// 1.13c D2Game+0x396E0 (checked against disassembly, no delta)
int32_t __fastcall MISSMODE_SrvHit55_Baalnferno(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_PLAYER)
    {
        return 2;
    }

    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->dwHitPar[0] <= 0)
    {
        return 1;
    }

    const int32_t nCurrentMana = D2_CALL(STATLIST_UnitGetStatValue, pUnit, STAT_MANA, 0);
    if (nCurrentMana <= 0)
    {
        return 1;
    }

    const int32_t nPercentage = D2Clamp(pMissilesTxtRecord->dwHitPar[0], 1u, 100u);
    const int32_t nLostMana = D2Clamp(D2_CALL(MONSTERUNIQUE_CalculatePercentage, nCurrentMana, nPercentage, 100), 1, nCurrentMana);
    D2_CALL(STATLIST_SetUnitStat, pUnit, STAT_MANA, nCurrentMana - nLostMana, 0);
    return 2;
}

//D2Game.0x6FC5E370
// 1.13c D2Game+0x3B210 (area damage called directly, the wrapper is inlined; HitSubMissile test is a signed short)
int32_t __fastcall MISSMODE_SrvHit56_ArmageddonControl(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);

    const int32_t nX = CLIENTS_GetUnitX(pMissile);
    const int32_t nY = CLIENTS_GetUnitY(pMissile);

    D2DamageStrc damage = {};
    MISSMODE_FillDamageParams(pMissile, 0, &damage);

    int32_t nRange = pMissilesTxtRecord->dwHitPar[0];
    if (nRange <= 0)
    {
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (!pSkillsTxtRecord)
        {
            return 1;
        }

        nRange = std::max(SKILLS_EvaluateSkillFormula(pOwner, pSkillsTxtRecord->dwAuraRangeCalc, nSkillId, nLevel), 1);
    }

    damage.dwHitFlags |= pMissilesTxtRecord->dwHitFlags;
    damage.wResultFlags |= pMissilesTxtRecord->wResultFlags;
    // retail inlines the area-damage wrapper: the unit-in-range walk with filter 0x8583 and the area-damage callback
    sub_6FD0FE80(pGame, pOwner, nX, nY, nRange, 0x8583u, sub_6FCF5DE0, &damage, 0, __FILE__, __LINE__);

    if ((int16_t)pMissilesTxtRecord->wHitSubMissile[0] >= 0)
    {
        D2GAME_CreateMissile_6FD115E0(pGame, pOwner, nSkillId, nLevel, pMissilesTxtRecord->wHitSubMissile[0], nX, nY);
    }

    return 1;
}

//D2Game.0x6FC5E530
// 1.13c D2Game+0x3BED0 (HitPar clamp signed: cmp/jge, was unsigned)
int32_t __fastcall MISSMODE_SrvHit58_BaalTauntLightningControl(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    SEED_InitLowSeed(&pMissile->pSeed, CLIENTS_GetUnitX(pMissile));
    const int32_t nParam = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);

    D2MissileStrc missileParams = {};
    missileParams.dwFlags = 32;
    missileParams.pOwner = pOwner;
    missileParams.pOrigin = pMissile;
    missileParams.nSkill = MISSILE_GetSkill(pMissile);
    missileParams.nSkillLevel = MISSILE_GetLevel(pMissile);
    missileParams.nMissile = pMissilesTxtRecord->wHitSubMissile[0];

    const int32_t nRange = 2 * nParam + 1;
    missileParams.nTargetX = CLIENTS_GetUnitX(pMissile) + ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - nParam;
    missileParams.nTargetY = CLIENTS_GetUnitY(pMissile) + ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, nRange) - nParam;

    MISSILES_CreateMissileFromParams(pGame, &missileParams);
    return 1;
}

//D2Game.0x6FC5E760
// 1.13c D2Game+0x3D800 (missile-hit slot 59; +0x3DA60 is slot 25, PantherPotGreen). Not yet matched: retail returns 1 on every path and spawns
// through 0x3D680 (the poison-cloud spawner) with the hit-par count, not through the PantherPotGreen spawner.
int32_t __fastcall MISSMODE_SrvHit59_BaalTauntPoisonControl(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wHitSubMissile[0] < 0)
    {
        return 1;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return 1;
    }

    const int32_t nHitPar = std::max(int32_t(pMissilesTxtRecord->dwHitPar[0]), 1);
    MISSMODE_CreatePantherPotGreenSubmissiles(pGame, pOwner, pMissile, pMissilesTxtRecord->wHitSubMissile[0], MISSILE_GetSkill(pMissile), MISSILE_GetLevel(pMissile), nHitPar);
    return 3;
}

//D2Game.0x6FC5E7F0
// 1.13c D2Game+0x39DA0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit23_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pMissile)
    {
        return 1;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 1;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    if (nSkillId > 0 && nLevel > 0 && SUNIT_GetOwner(pGame, pMissile))
    {
        if (pUnit)
        {
            UNITS_SetTargetUnitForDynamicUnit(pMissile, pUnit);
        }
        sub_6FD14400(pGame, pMissile, pMissilesTxtRecord->dwHitPar[0], nSkillId, nLevel);
    }
    return 1;
}

//D2Game.0x6FC5E890
// 1.13c D2Game+0x39CE0 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg01_FireArrow_MagicArrow_ColdArrow(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    int32_t nPercentage = MISSILE_EvaluateMissileFormula(pMissile, SUNIT_GetOwner(pGame, pMissile), pMissilesTxtRecord->dwDmgCalc, -1, -1);
    if (nPercentage <= 0)
    {
        return;
    }

    nPercentage = std::min(nPercentage, 100);

    const int32_t nDamageReduction = std::min(MONSTERUNIQUE_CalculatePercentage(pDamage->dwPhysDamage, nPercentage, 100), pDamage->dwPhysDamage);

    pDamage->dwPhysDamage -= nDamageReduction;
    MISSMODE_AddDamageValue(pGame, pMissile, pUnit, pDamage, nDamageReduction);
}

//D2Game.0x6FC5E9D0
// 1.13c D2Game+0x3A850 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg12_LightningJavelin(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    int32_t nPercentage = MISSILE_EvaluateMissileFormula(pMissile, SUNIT_GetOwner(pGame, pMissile), pMissilesTxtRecord->dwDmgCalc, -1, -1);
    if (nPercentage <= 0)
    {
        return;
    }

    nPercentage = std::min(nPercentage, 100);

    const int32_t nDamageBonus = std::min(MONSTERUNIQUE_CalculatePercentage(pDamage->dwPhysDamage, nPercentage, 100), pDamage->dwPhysDamage);
    const int32_t nFinalDamage = MISSMODE_GetDamageValue(pGame, pMissile, pUnit, pDamage) + nDamageBonus;
    MISSMODE_ResetDamageParams(pGame, pMissile, pDamage);
    MISSMODE_AddDamageValue(pGame, pMissile, pUnit, pDamage, nFinalDamage);
}

//D2Game.0x6FC5EB20
// 1.13c D2Game+0x39080 (DmgParam1 is passed to the percentage unclamped; the 1.10f max(.., 0) is gone)
void __fastcall MISSMODE_SrvDmg02_IceArrow_RoyalStrikeChaos(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (pMissilesTxtRecord)
    {
        pDamage->dwFrzLen = MONSTERUNIQUE_CalculatePercentage(STATLIST_UnitGetStatValue(pMissile, STAT_COLDLENGTH, 0), pMissilesTxtRecord->dwDmgParam[0], 100);
        pDamage->dwColdLen = 0;
    }
}

//D2Game.0x6FC5EC70
// 1.13c D2Game+0x39670 (the roll & 127 is compared signed against DmgParam1)
void __fastcall MISSMODE_SrvDmg03_Blaze_FireWall_ImmolationFire_MeteorFire(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (pMissile)
    {
        D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
        if (pMissilesTxtRecord && (int32_t)(ITEMS_RollRandomNumber(&pMissile->pSeed) & 127) < pMissilesTxtRecord->dwDmgParam[0])
        {
            pDamage->wResultFlags |= 0x4000u;
        }
    }
}

//D2Game.0x6FC5ECE0
// 1.13c D2Game+0x38E70 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg04_IceBlast(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    pDamage->dwFrzLen = pDamage->dwColdLen;
    pDamage->dwColdLen = 0;
}

//D2Game.0x6FC5ED00
// 1.13c D2Game+0x39040 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg10_BladesOfIceCubes(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (pMissile && SKILLS_GetMissilesTxtRecord(pMissile->dwClassId))
    {
        pDamage->dwFrzLen = pDamage->dwColdLen;
    }
}

//D2Game.0x6FC5ED50
// 1.13c D2Game+0x3A790 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg05_BlessedHammer(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    const int32_t nBaseDamage = MISSMODE_GetDamageValue(pGame, pMissile, pUnit, pDamage);
    if (pMissilesTxtRecord->dwDmgParam[0] > 0 && MONSTERS_IsUndead(pUnit))
    {
        MISSMODE_AddDamageValue(pGame, pMissile, pUnit, pDamage, MONSTERUNIQUE_CalculatePercentage(nBaseDamage, pMissilesTxtRecord->dwDmgParam[0], 100));
    }

    if (pMissilesTxtRecord->dwDmgParam[1] > 0 && MONSTERS_IsDemon(pUnit))
    {
        MISSMODE_AddDamageValue(pGame, pMissile, pUnit, pDamage, MONSTERUNIQUE_CalculatePercentage(nBaseDamage, pMissilesTxtRecord->dwDmgParam[1], 100));
    }
}

//D2Game.0x6FC5EF40
// 1.13c D2Game+0x39C80 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg06_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner && !(pOwner->dwFlags & UNITFLAG_ISMERC) && pUnit)
    {
        pUnit->dwFlags |= UNITFLAG_NOTC;
    }
}

//D2Game.0x6FC5EF80
// 1.13c D2Game+0x39600 (checked against decompile, no delta; the stun formula is the 0x1B10 helper inlined)
void __fastcall MISSMODE_SrvDmg07_Warcry_ShockWave(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    const int32_t nLevel = MISSILE_GetLevel(pMissile);
    if (pMissilesTxtRecord->dwDmgParam[0] > 0)
    {
        pDamage->dwStunLen = pMissilesTxtRecord->dwDmgParam[0];
    }
    else
    {
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (pSkillsTxtRecord && nLevel > 0)
        {
            pDamage->dwStunLen = pSkillsTxtRecord->dwParam[0] + (nLevel - 1) * pSkillsTxtRecord->dwParam[1];
        }
        else
        {
            pDamage->dwStunLen = 0;
        }
    }
    pDamage->dwHitClass = 0x60u;
}

//D2Game.0x6FC5F060
// 1.13c D2Game+0x38FF0 (ProgOverlay is tested as a signed short)
void __fastcall MISSMODE_SrvDmg08_EruptionCrack(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (pMissile)
    {
        D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
        if (pMissilesTxtRecord && pUnit && (int16_t)pMissilesTxtRecord->wProgOverlay > 0)
        {
            UNITS_SetOverlay(pUnit, pMissilesTxtRecord->wProgOverlay, 0);
        }
    }
}

//D2Game.0x6FC5F0C0
// 1.13c D2Game+0x39560 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg09_Twister(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    const int32_t nSkillId = MISSILE_GetSkill(pMissile);
    if (pMissilesTxtRecord->dwDmgParam[0] > 0)
    {
        pDamage->dwStunLen = pMissilesTxtRecord->dwDmgParam[0];
    }
    else
    {
        D2SkillsTxt* pSkillsTxtRecord = SKILLS_GetSkillsTxtRecord(nSkillId);
        if (pSkillsTxtRecord)
        {
            pDamage->dwStunLen = pSkillsTxtRecord->dwParam[1];
        }
        else
        {
            pDamage->dwStunLen = 0;
        }
    }
    pDamage->dwHitClass = 0x60u;
}

//D2Game.0x6FC5F170
// 1.13c D2Game+0x39BD0 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg11_RabiesContagion(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (pOwner && pUnit)
    {
        const int32_t nSkillId = MISSILE_GetSkill(pMissile);
        const int32_t nLevel = MISSILE_GetLevel(pMissile);
        const int32_t nPoisonLength = MISSILE_GetTargetX(pMissile) - pGame->dwGameFrame;
        if (nPoisonLength >= 10 && nPoisonLength <= SKILLS_GetElementalLength(pOwner, nSkillId, nLevel, 1))
        {
            pDamage->dwPoisLen = nPoisonLength;
            return;
        }
    }

    pDamage->dwPoisDamage = 0;
    pDamage->dwPoisLen = 0;
}

//D2Game.0x6FC5F1F0
// 1.13c D2Game+0x3AAA0 (checked against decompile, no delta)
void __fastcall MISSMODE_SrvDmg13_BlessedHammerEx(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER)
    {
        D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
        if (pOwner)
        {
            pDamage->dwPhysDamage += (int32_t)(pDamage->dwPhysDamage * STATLIST_UnitGetStatValue(pOwner, STAT_DAMAGEPERCENT, 0)) / 100;
        }

        MISSMODE_SrvDmg05_BlessedHammer(pGame, pMissile, pUnit, pDamage);
    }
}

// Prior art: D2MOO MoltenBoulder damage callback; retail uses the flag helper.
// 1.13c D2Game+0x39A40 (checked against decompile, no delta)
D2SrvDmg14Seam g_d2SrvDmg14;
// 1.13c D2Game+0x39A40: fastcall ECX = pGame, EDX = pMissile, stack = pUnit, pDamage (ret 8); the roll is RollLimited(&missile seed, 100)
void __fastcall MISSMODE_SrvDmg14_MoltenBoulder(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, D2DamageStrc* pDamage)
{
    if (!pMissile || pMissile->dwUnitType != UNIT_MISSILE)
    {
        return;
    }
    D2MissilesTxt* pRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pRecord || !pUnit)
    {
        return;
    }
    const int32_t nParam = pRecord->dwDmgParam[0];
    uint32_t nMultiplier = 1;
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        if (nParam < 1) return;
    }
    else if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        auto flag = [](int32_t nId, int32_t nFlag) { return g_d2SrvDmg14.flag ? g_d2SrvDmg14.flag(nId, nFlag) : (uint32_t)MONSTERREGION_CheckMonStats2Flag(nId, nFlag); };
        const bool bSmall = flag(pUnit->dwClassId, 10) != 0;
        if (nParam < 1)
        {
            if (!bSmall) return;
        }
        else if (nParam == 1)
        {
            if (bSmall) nMultiplier = 2;
            else if (flag(pUnit->dwClassId, 11)) return;
        }
        else
        {
            if (bSmall) nMultiplier = 3;
            else if (!flag(pUnit->dwClassId, 11)) nMultiplier = 2;
        }
    }
    else return;
    const int32_t nChance = int32_t(uint32_t(pRecord->dwDmgParam[1]) * nMultiplier);
    if (nChance > 0 && int32_t(g_d2SrvDmg14.roll ? g_d2SrvDmg14.roll(&pMissile->pSeed, 100) : ITEMS_RollLimitedRandomNumber(&pMissile->pSeed, 100)) < nChance)
    {
        pDamage->wResultFlags |= 8u;
    }
}

//D2Game.0x6FC5F4E0
// 1.13c D2Game+0x3AE10 (checked against decompile, no delta)
void __fastcall MISSMODE_SetDamageFlags(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pDefender, D2DamageStrc* pDamage)
{
    if (!pMissile || pMissile->dwUnitType != UNIT_MISSILE)
    {
        return;
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
    if (!pOwner)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return;
    }

    pDamage->wResultFlags |= DAMAGERESULTFLAG_SUCCESSFULHIT;

    if (!STATES_CheckState(pDefender, STATE_UNINTERRUPTABLE))
    {
        if (pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_GETHIT])
        {
            pDamage->wResultFlags |= DAMAGERESULTFLAG_GETHIT;
        }
        else if (pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_SOFTHIT])
        {
            pDamage->wResultFlags |= DAMAGERESULTFLAG_SOFTHIT;
        }

        if (pMissilesTxtRecord->nKnockBack && (ITEMS_RollRandomNumber(&pMissile->pSeed) % 100) < pMissilesTxtRecord->nKnockBack)
        {
            pDamage->wResultFlags |= DAMAGERESULTFLAG_KNOCKBACK;
        }
    }

    uint32_t nBlockFlags = SUNITDMG_ApplyBlockOrDodge(pGame, pOwner, pDefender, 1, pDamage->dwPhysDamage != 0);
    if (nBlockFlags & 0x1F && !(pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_GETHIT]) && !pMissilesTxtRecord->nKnockBack)
    {
        pDamage->wResultFlags = pDamage->wResultFlags & (uint16_t)~DAMAGERESULTFLAG_SUCCESSFULHIT | DAMAGERESULTFLAG_SOFTHIT;
        nBlockFlags &= 0xFFFFFFE0;
    }

    if (nBlockFlags & 8)
    {
        pDamage->wResultFlags |= DAMAGERESULTFLAG_EVADE;
    }

    if (nBlockFlags & 2)
    {
        pDamage->wResultFlags |= DAMAGERESULTFLAG_AVOID;
    }

    if (nBlockFlags & 0x11)
    {
        pDamage->wResultFlags |= DAMAGERESULTFLAG_BLOCK;
    }

    if (nBlockFlags)
    {
        pDamage->wResultFlags &= (uint16_t)~DAMAGERESULTFLAG_SUCCESSFULHIT;
    }

    pDamage->dwHitClass = pDamage->dwHitClass & 0xF0 | MISSILE_GetHitClass(pMissile);

    if (pDamage->dwHitClass & 0xF0)
    {
        pDamage->nHitClassActiveSet = 1;
    }

    D2GAME_MONSTER_ApplyCriticalDamage_6FC62E70(pOwner, pDefender, pDamage);

    if (MISSILE_GetFlags(pMissile) & 1)
    {
        pDamage->dwHitFlags |= DAMAGEHITFLAG_32;
    }

    if (MISSILE_GetFlags(pMissile) & 2)
    {
        pDamage->dwHitFlags |= DAMAGEHITFLAG_128;
    }

    pDamage->dwPiercePct = STATLIST_UnitGetStatValue(pMissile, STAT_DAMAGE_FRAMERATE, 0);

    if (pDamage->wResultFlags & DAMAGERESULTFLAG_SUCCESSFULHIT)
    {
        SUNITDMG_ExecuteEvents(pGame, pOwner, pDefender, 1, pDamage);
    }

    SUNITDMG_ExecuteMissileDamage(pGame, pOwner, pDefender, pDamage);
}

//D2Game.0x6FC5F6C0
// 1.13c D2Game+0x3F6D0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo19_RadamentDeath(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    const int32_t nCurrentFrame = MISSILE_GetCurrentFrame(pMissile);
    if (nCurrentFrame > 1 && nCurrentFrame < 25)
    {
        sub_6FD14BD0(pGame, 0x3002, CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), pMissile, 124, 1, (void*)MISSMODE_RadamentDeathAreaEffectCallback);   // d2-web: MSVC fn -> void*
    }
    else if (nCurrentFrame == 1)
    {
        sub_6FD14BD0(pGame, 0x3002, CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), pMissile, 124, 1, (void*)MISSMODE_RadamentDeathAreaEffectCallbackFirstFrame);   // d2-web: MSVC fn -> void*
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC5F7C0
// 1.13c D2Game+0x39930 (the redemption effect inlined: chance formula evaluated first, then RollLimited(100) compared signed)
int32_t __fastcall MISSMODE_RadamentDeathAreaEffectCallback(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pCorpse, int32_t nSkillLevel, int32_t nUnused)
{
    D2SkillsTxt* pSkillsTxtRecord = D2_CALL(SKILLS_GetSkillsTxtRecord, SKILL_REDEMPTION);
    if (!pSkillsTxtRecord || !pCorpse || !D2_CALL(SKILLS_CanUnitCorpseBeSelected, pCorpse))
    {
        return 0;
    }

    const int32_t nChance = D2_CALL(SKILLS_EvaluateSkillFormula, pUnit, pSkillsTxtRecord->dwCalc[0], SKILL_REDEMPTION, nSkillLevel);
    if ((int32_t)D2_CALL(ITEMS_RollLimitedRandomNumber, &pUnit->pSeed, 100) >= nChance)
    {
        return 0;
    }

    const int32_t nLife = D2_CALL(SKILLS_EvaluateSkillFormula, pUnit, pSkillsTxtRecord->dwCalc[1], SKILL_REDEMPTION, nSkillLevel);
    const int32_t nMaxLife = D2_CALL(STATLIST_GetMaxLifeFromUnit, pUnit);
    const int32_t nLifeNow = D2_CALL(STATLIST_UnitGetStatValue, pUnit, STAT_HITPOINTS, 0);
    D2_CALL(STATLIST_SetUnitStat, pUnit, STAT_HITPOINTS, std::min(nLifeNow + (nLife << 8), nMaxLife), 0);

    const int32_t nMana = D2_CALL(SKILLS_EvaluateSkillFormula, pUnit, pSkillsTxtRecord->dwCalc[2], SKILL_REDEMPTION, nSkillLevel);
    const int32_t nMaxMana = D2_CALL(STATLIST_GetMaxManaFromUnit, pUnit);
    const int32_t nManaNow = D2_CALL(STATLIST_UnitGetStatValue, pUnit, STAT_MANA, 0);
    D2_CALL(STATLIST_SetUnitStat, pUnit, STAT_MANA, std::min(nManaNow + (nMana << 8), nMaxMana), 0);

    D2_CALL(STATES_ToggleState, pCorpse, STATE_REDEEMED, 1);
    pCorpse->dwFlags &= ~(UNITFLAG_TARGETABLE | UNITFLAG_CANBEATTACKED);
    return 1;
}

//D2Game.0x6FC5F7E0
// 1.13c D2Game+0x39910 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_RadamentDeathAreaEffectCallbackFirstFrame(D2GameStrc* pGame, D2UnitStrc* pUnit, D2UnitStrc* pCorpse, int32_t nSkillLevel, int32_t nUnused)
{
    return SKILLS_ApplyRedemptionEffect(pGame, pUnit, pCorpse, SKILL_REDEMPTION, nSkillLevel, 1);
}

//D2Game.0x6FC5F800
// 1.13c D2Game+0x3F670
int32_t __fastcall MISSMODE_SrvDo36_BaalFxControl(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    const int32_t nFrame = D2_CALL(MISSILE_GetCurrentFrame, pMissile);
    if (!D2_CALL(MISSILE_GetTargetX, pMissile) && nFrame <= 100)
    {
        D2_CALL(MISSILE_SetTargetX, pMissile, 1);
        D2ActiveRoomStrc* pRoom = D2_CALL(UNITS_GetRoom, pMissile);
        if (D2_CALL(QUESTS_CheckNotIntroQuest, pGame, 36))
        {
            D2_CALL(ACT5Q6_SpawnTyrael, pGame, pRoom, pMissile);
        }

        D2_CALL(DUNGEON_ToggleHasPortalFlag, pRoom, 1);
    }

    return D2_CALL(MISSMODE_HandleMissileCollision, pGame, pMissile);
}

//D2Game.0x6FC5F860
// 1.13c D2Game+0x3BBB0 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvHit57_BaalFxControl(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    if (!pUnit && !MISSILE_GetTargetX(pMissile))
    {
        MISSILE_SetTargetX(pMissile, 1);
        D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);
        if (QUESTS_CheckNotIntroQuest(pGame, 36))
        {
            ACT5Q6_SpawnTyrael(pGame, pRoom, pMissile);
        }

        DUNGEON_ToggleHasPortalFlag(pRoom, 1);
    }

    return 0;
}

//D2Game.0x6FC5F8B0
// 1.13c D2Game+0x3F550 (SubMissile ids are tested as signed shorts)
int32_t __fastcall MISSMODE_SrvDo37_Unused(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    if (!pMissile)
    {
        return 2;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord)
    {
        return 2;
    }

    if ((int16_t)pMissilesTxtRecord->wSubMissile[0] > 0 &&MISSILE_GetRemainingFrames(pMissile) == pMissilesTxtRecord->dwParam[0])
    {
        D2GAME_CreateMissile_6FD115E0(pGame, SUNIT_GetOwner(pGame, pMissile), MISSILE_GetSkill(pMissile), MISSILE_GetLevel(pMissile),
                                      pMissilesTxtRecord->wSubMissile[0], CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile));
    }

    if ((int16_t)pMissilesTxtRecord->wSubMissile[1] > 0 &&MISSILE_GetRemainingFrames(pMissile) == pMissilesTxtRecord->dwParam[1])
    {
        D2GAME_CreateMissile_6FD115E0(pGame, SUNIT_GetOwner(pGame, pMissile), MISSILE_GetSkill(pMissile), MISSILE_GetLevel(pMissile),
                                      pMissilesTxtRecord->wSubMissile[1], CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile));
    }

    if ((int16_t)pMissilesTxtRecord->wSubMissile[2] > 0 &&MISSILE_GetRemainingFrames(pMissile) == pMissilesTxtRecord->dwParam[2])
    {
        D2GAME_CreateMissile_6FD115E0(pGame, SUNIT_GetOwner(pGame, pMissile), MISSILE_GetSkill(pMissile), MISSILE_GetLevel(pMissile),
                                      pMissilesTxtRecord->wSubMissile[2], CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile));
    }

    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

//D2Game.0x6FC5F8C0
// 1.13c D2Game+0x3F540 (checked against decompile, no delta)
int32_t __fastcall MISSMODE_SrvDo01_BasicMissile(D2GameStrc* pGame, D2UnitStrc* pMissile)
{
    return MISSMODE_HandleMissileCollision(pGame, pMissile);
}

// 1.13c D2Game+0x3E590: the just-hit delay state the hit handler puts on its target (1.10f inlined it twice)
static void MISSMODE_SetJustHitState(D2UnitStrc* pMissile, D2UnitStrc* pUnit)
{
    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord->nNextHit || !pUnit)
    {
        return;
    }

    const int32_t nDelay = pMissile->pGame->dwGameFrame + pMissilesTxtRecord->nNextDelay;
    D2StatListStrc* pStatList = STATLIST_AllocStatList(pMissile->pGame->pMemoryPool, 2u, nDelay, pUnit->dwUnitType, pUnit->dwUnitId);
    if (pStatList)
    {
        EVENT_SetEvent(pMissile->pGame, pUnit, EVENTTYPE_REMOVESTATE, nDelay, 0, 0);
        STATLIST_SetState(pStatList, STATE_JUSTHIT);
        STATLIST_SetStatRemoveCallback(pStatList, MISSMODE_ToggleStateOff);
        D2COMMON_10475_PostStatToStatList(pUnit, pStatList, 1);
        STATES_ToggleState(pUnit, STATE_JUSTHIT, 1);
    }
}

//D2Game.0x6FC5FAD0
// 1.13c D2Game+0x3EDE0 (hit/damage slots are signed shorts bounded by the 71/31-entry tables; the just-hit state is the
// 0x3E590 helper, the target-AC drain after the damage flags is the 0x3AFF0 tail)
int32_t __fastcall MISSMODE_SrvDmgHitHandler(D2GameStrc* pGame, D2UnitStrc* pMissile, D2UnitStrc* pUnit, int32_t a4)
{
    using MissileDmgFunc = void(__fastcall*)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, D2DamageStrc*);
    constexpr MissileDmgFunc gpMissileSrvDmgFnTable_6FD2E838[] =
    {
        nullptr,
        MISSMODE_SrvDmg01_FireArrow_MagicArrow_ColdArrow,
        MISSMODE_SrvDmg02_IceArrow_RoyalStrikeChaos,
        MISSMODE_SrvDmg03_Blaze_FireWall_ImmolationFire_MeteorFire,
        MISSMODE_SrvDmg04_IceBlast,
        MISSMODE_SrvDmg05_BlessedHammer,
        MISSMODE_SrvDmg06_Unused,
        MISSMODE_SrvDmg07_Warcry_ShockWave,
        MISSMODE_SrvDmg08_EruptionCrack,
        MISSMODE_SrvDmg09_Twister,
        MISSMODE_SrvDmg10_BladesOfIceCubes,
        MISSMODE_SrvDmg11_RabiesContagion,
        MISSMODE_SrvDmg12_LightningJavelin,
        MISSMODE_SrvDmg13_BlessedHammerEx,
        MISSMODE_SrvDmg14_MoltenBoulder,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };

    using MissileHitFunc = int32_t(__fastcall*)(D2GameStrc*, D2UnitStrc*, D2UnitStrc*);
    constexpr MissileHitFunc gpMissileSrvHitFnTable_6FD2E718[] =
    {
        nullptr,
        MISSMODE_SrvHit01_Fireball_ExplodingArrow_FreezingArrowExplosion,
        MISSMODE_SrvHit02_PlagueJavelin_PoisonPotion,
        MISSMODE_SrvHit03_ExplosivePotion_BombOnGround,
        MISSMODE_SrvHit04_ExplodingArrow_FreezingArrow_RoyalStrikeMeteorCenter,
        MISSMODE_SrvHit05_Unused,
        MISSMODE_SrvHit06_Unused,
        MISSMODE_SrvHit07_HolyBolt_FistOfTheHeavenBolt,
        MISSMODE_SrvHit08_Blaze,
        MISSMODE_SrvHit09_ImmolationArrow,
        MISSMODE_SrvHit10_GuidedArrow_BoneSpirit,
        MISSMODE_SrvHit11_Unused,
        MISSMODE_SrvHit12_ChainLightning_LightningStrike,
        MISSMODE_SrvHit13_GlacialSpike_HellMeteorDown,
        MISSMODE_SrvHit14_MeteorCenter_CatapultMeteor_RoyalStrikeMeteor,
        MISSMODE_SrvHit15_SpiderGooLay,
        MISSMODE_SrvHit16_SpiderGoo_VinesTrail_VinesWither,
        MISSMODE_SrvHit17_Howl,
        MISSMODE_SrvHit18_Shout_BattleCommand_BattleOrders,
        MISSMODE_SrvHit19_FingerMageSpider,
        MISSMODE_SrvHit20_LightningFury,
        MISSMODE_SrvHit21_BattleCry,
        MISSMODE_SrvHit22_FistOfTheHeavensDelay,
        MISSMODE_SrvHit23_Unused,
        MISSMODE_SrvHit24_PantherPotOrange,
        MISSMODE_SrvHit25_PantherPotGreen,
        MISSMODE_SrvHit26_GrimWardStart,
        MISSMODE_SrvHit27_GrimWard,
        MISSMODE_SrvHit28_GrimWardScare,
        MISSMODE_SrvHit29_FrozenOrb,
        nullptr,
        MISSMODE_SrvHit31_FireHead,
        MISSMODE_SrvHit32_CairnStones,
        MISSMODE_SrvHit33_TowerChestSpawner,
        nullptr,
        MISSMODE_SrvHit35_OrbMist,
        MISSMODE_SrvHit36_MissileInAir,
        MISSMODE_SrvHit37_BladeCreeper,
        MISSMODE_SrvHit38_CatapultChargedBall,
        MISSMODE_SrvHit39_ImpSpawnMonsters,
        MISSMODE_SrvHit40_CatapultSpikeBall,
        nullptr,
        nullptr,
        MISSMODE_SrvHit43_HealingVortex,
        MISSMODE_SrvHit44_ExplodingJavelin,
        MISSMODE_SrvHit45_LightningTrailingJavelin,
        nullptr,
        MISSMODE_SrvHit47_MoltenBoulder,
        MISSMODE_SrvHit48_MoltenBoulderEmerge,
        nullptr,
        MISSMODE_SrvHit50_PlagueVinesTrail,
        MISSMODE_SrvHit51_VolcanoDebris,
        MISSMODE_SrvHit52_BladeFury,
        MISSMODE_SrvHit53_RabiesContagion,
        MISSMODE_SrvHit54_BaalSpawnMonsters,
        MISSMODE_SrvHit55_Baalnferno,
        MISSMODE_SrvHit56_ArmageddonControl,
        MISSMODE_SrvHit57_BaalFxControl,
        MISSMODE_SrvHit58_BaalTauntLightningControl,
        MISSMODE_SrvHit59_BaalTauntPoisonControl,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };

    D2MissilesTxt* pMissilesTxtRecord = nullptr;
    if (pMissile)
    {
        pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    }

    D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);

    int32_t nHitResult = 2;
    int32_t nPierceFlags = 3;

    if (pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_EXPLOSION])
    {
        nHitResult = 0;
    }

    if (pUnit)
    {
        if ((pMissilesTxtRecord->nNextHit && STATES_CheckState(pUnit, STATE_JUSTHIT)) || MISSILE_CheckUnitIfOwner(pMissile, pUnit))
        {
            return 1;
        }

        MISSILE_SetOwner(pMissile, pUnit);

        const int32_t nMissileMode = pMissile ? pMissile->dwMissileMode : MISSMODE_NOCOLLIDE;
        switch (nMissileMode)
        {
        case MISSMODE_NOCOLLIDE:
            return 1;
        case MISSMODE_PLAYERKILL:
            if (pUnit->dwUnitType != UNIT_PLAYER)
            {
                return 1;
            }
            break;
        case MISSMODE_MONSTERKILL:
            if (pUnit->dwUnitType != UNIT_MONSTER)
            {
                return 1;
            }
            break;
        default:
            break;
        }

        if (pOwner)
        {
            if (!sub_6FCBD900(pGame, pOwner, pUnit) && !pMissilesTxtRecord->nCollideFriend)
            {
                return 1;
            }

            if (pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_PIERCE] && (STATLIST_UnitGetStatValue(pOwner, STAT_SKILL_PIERCE, 0) || STATLIST_UnitGetItemStatOrSkillStatValue(pOwner, STAT_ITEM_PIERCE, 0)))
            {
                nPierceFlags = 3;

                const int32_t nPierceIndex = STATLIST_UnitGetStatValue(pMissile, STAT_PIERCE_IDX, 0);
                if (nPierceIndex)
                {
                    nPierceFlags = 2;
                    STATLIST_SetUnitStat(pMissile, STAT_PIERCE_IDX, nPierceIndex - 1, 0);
                }
            }
        }
    }

    if (pMissilesTxtRecord->nCollideKill)
    {
        nHitResult |= 1u;
    }

    if (pUnit)
    {
        if (pMissilesTxtRecord->nToHit)
        {
            int32_t nToHit = 0;
            if (pMissile)
            {
                nToHit = STATLIST_UnitGetStatValue(pMissile, STAT_TOHIT, 0);
            }

            if (!SUNITDMG_IsHitSuccessful(pOwner, pUnit, nToHit, 1))
            {
                MISSMODE_SetJustHitState(pMissile, pUnit);
                SUNITEVENT_Trigger(pGame, UNITEVENT_HITBYMISSILE, pUnit, pMissile, nullptr);

                if (pMissilesTxtRecord->nAlwaysExplode)
                {
                    const int16_t nHitFunc = (int16_t)pMissilesTxtRecord->wSrvHitFunc;
                    if (nHitFunc > 0 && nHitFunc < (int16_t)std::size(gpMissileSrvHitFnTable_6FD2E718))
                    {
                        if (gpMissileSrvHitFnTable_6FD2E718[nHitFunc](pGame, pMissile, pUnit) & 4)
                        {
                            return 1;
                        }
                    }
                }

                return 2;
            }
        }
    }
    else
    {
        if (!a4 && !pMissilesTxtRecord->nAlwaysExplode)
        {
            if (!(nHitResult & 1) && !a4 || !(nPierceFlags & 1))
            {
                return 1;
            }

            D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);
            if (pRoom)
            {
                COLLISION_ResetMaskWithSize(pRoom, CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), UNITS_GetUnitSizeX(pMissile), 0x40u);
            }

            return 2;
        }
    }

    if (nPierceFlags & 2)
    {
        MISSMODE_SetJustHitState(pMissile, pUnit);
        SUNITEVENT_Trigger(pGame, UNITEVENT_HITBYMISSILE, pUnit, pMissile, 0);

        const int16_t nHitFunc = (int16_t)pMissilesTxtRecord->wSrvHitFunc;
        if (nHitFunc > 0 && nHitFunc < (int16_t)std::size(gpMissileSrvHitFnTable_6FD2E718))
        {
            nHitResult = gpMissileSrvHitFnTable_6FD2E718[nHitFunc](pGame, pMissile, pUnit);
            if (nHitResult & 4)
            {
                return 1;
            }
        }
    }

    if (pUnit && nHitResult & 2)
    {
        D2DamageStrc damage = {};
        MISSMODE_FillDamageParams(pMissile, pUnit, &damage);
        
        const int16_t nDmgFunc = (int16_t)pMissilesTxtRecord->wSrvDmgFunc;
        if (nDmgFunc > 0 && nDmgFunc < (int16_t)std::size(gpMissileSrvDmgFnTable_6FD2E838))
        {
            gpMissileSrvDmgFnTable_6FD2E838[nDmgFunc](pGame, pMissile, pUnit, &damage);
        }

        MISSMODE_SetDamageFlags(pGame, pMissile, pUnit, &damage);

        if (pUnit->dwUnitType == UNIT_MONSTER && !MONSTERS_GetHirelingTypeId(pUnit))
        {
            const int32_t nDamageTargetAc = STATLIST_UnitGetStatValue(pMissile, STAT_ITEM_DAMAGETARGETAC, 0);
            if (nDamageTargetAc)
            {
                const int32_t nArmor = std::max(nDamageTargetAc + STATLIST_UnitGetStatValue(pUnit, STAT_ARMORCLASS, 0), 0);
                STATLIST_SetUnitStat(pUnit, STAT_ARMORCLASS, nArmor, 0);
            }
        }
    }

    if (!(nHitResult & 1) && !a4 || !(nPierceFlags & 1))
    {
        return 1;
    }

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);
    if (pRoom)
    {
        COLLISION_ResetMaskWithSize(pRoom, CLIENTS_GetUnitX(pMissile), CLIENTS_GetUnitY(pMissile), UNITS_GetUnitSizeX(pMissile), 0x40u);
    }

    return 2;
}

//D2Game.0x6FC60080
// 1.13c D2Game+0x38E60 (checked against disassembly, no delta)
void __fastcall MISSMODE_ToggleStateOff(D2UnitStrc* pUnit, int32_t nState, D2StatListStrc* pStatList)
{
    D2_MAYBE_UNUSED(pStatList);
    STATES_ToggleState(pUnit, nState, 0);
}

//D2Game.0x6FC60090
// 1.13c D2Game+0x3E3F0
void __fastcall MISSMODE_SrvDoHandler(D2GameStrc* pGame, D2UnitStrc* pMissile, D2C_EventTypes nEventType)
{
	D2_MAYBE_UNUSED(nEventType);

    using MissileSrvDoFunc = int32_t(__fastcall*)(D2GameStrc*, D2UnitStrc*);
    constexpr MissileSrvDoFunc gMissileSrvDoFuncTable[] =
    {
        nullptr,
        MISSMODE_SrvDo01_BasicMissile,
        MISSMODE_SrvDo02_PlagueJavelin_PoisonJavelin_PoisonTrap,
        MISSMODE_SrvDo03_PoisonCloud_Blizzard_ThunderStorm_HandOfGod,
        nullptr,
        MISSMODE_SrvDo05_FireWall_ImmolationFire_MeteorFire_MoltenBoulderFirePath,
        MISSMODE_SrvDo06_MoltenBoulder_FireWallMaker,
        MISSMODE_SrvDo07_GuidedArrow_BoneSpirit,
        MISSMODE_SrvDo08_MonBlizzCenter,
        MISSMODE_SrvDo09_BatLightningBolt,
        MISSMODE_SrvDo10_BlizzardCenter,
        MISSMODE_SrvDo11_FingerMageSpider,
        MISSMODE_SrvDo12_DiabWallMaker,
        MISSMODE_SrvDo13_BoneWallMaker,
        MISSMODE_SrvDo14_GrimWard,
        MISSMODE_SrvDo15_FrozenOrb,
        MISSMODE_SrvDo16_FrozenOrbNova,
        MISSMODE_SrvDo17_CairnStones,
        MISSMODE_SrvDo18_TowerChestSpawner,
        MISSMODE_SrvDo19_RadamentDeath,
        MISSMODE_SrvDo20_BladeCreeper,
        MISSMODE_SrvDo21_Distraction,
        MISSMODE_SrvDo22_LightningTrailingJavelin,
        MISSMODE_SrvDo23_24_SuccFireBall_FirestormMaker,
        MISSMODE_SrvDo23_24_SuccFireBall_FirestormMaker,
        MISSMODE_SrvDo25_EruptionCenter,
        MISSMODE_SrvDo26_Vines_PlagueVines,
        MISSMODE_SrvDo27_Tornado,
        MISSMODE_SrvDo28_Volcano,
        MISSMODE_SrvDo29_RecyclerDelay,
        MISSMODE_SrvDo30_RabiesPlague,
        MISSMODE_SrvDo31_WakeOfDestructionMaker_BaalColdMaker,
        MISSMODE_SrvDo32_TigerFury,
        MISSMODE_SrvDo33_VineRecyclerDelay,
        MISSMODE_SrvDo34_BaalTauntControl,
        MISSMODE_SrvDo35_RoyalStrikeChaosIce,
        MISSMODE_SrvDo36_BaalFxControl,
        MISSMODE_SrvDo37_Unused,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };

    if (!pMissile)
    {
        return;
    }

    D2MissilesTxt* pMissilesTxtRecord = SKILLS_GetMissilesTxtRecord(pMissile->dwClassId);
    if (!pMissilesTxtRecord || pMissilesTxtRecord->wSrvDoFunc <= 0 || pMissilesTxtRecord->wSrvDoFunc >= std::size(gMissileSrvDoFuncTable))
    {
        return;
    }

    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pMissile);

    if (pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_SRCTOWN])
    {
        D2UnitStrc* pOwner = SUNIT_GetOwner(pGame, pMissile);
        if (pOwner && DUNGEON_IsRoomInTown(UNITS_GetRoom(pOwner)))
        {
            SUNIT_RemoveUnit(pGame, pMissile);
            return;
        }
    }

    if (!(pMissilesTxtRecord->dwMissileFlags & gdwBitMasks[MISSILESFLAGINDEX_TOWN]) && DUNGEON_IsRoomInTown(pRoom))
    {
        SUNIT_RemoveUnit(pGame, pMissile);
        return;
    }

    if (gMissileSrvDoFuncTable[pMissilesTxtRecord->wSrvDoFunc](pGame, pMissile) == 2)
    {
        SUNIT_RemoveUnit(pGame, pMissile);
    }
}
