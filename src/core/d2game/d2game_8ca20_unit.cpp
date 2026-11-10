// 1.13c D2Game+0x8CA20 -- the monster-assign packet (0xAC) for one monster, sent to one client: what a client is told
// the first time a monster enters its rooms. This is the play-path form over the engine's own unit; the register-level
// form of the same function is d2game_8ca20.cpp.
// Prior art used: D2MOO 1.10f sub_6FC3FC80 (GAME/SCmd.cpp); 1.13c keeps its layout (13-byte header, 244-byte bit stream).
#include <cstdint>

#include <D2BitManip.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <D2StatList.h>
#include <Units/Units.h>
#include <D2PacketDef.h>
#include <GAME/Clients.h>
#include <GAME/SCmd.h>
#include <AI/AiGeneral.h>
#include <MONSTER/Monster.h>
#include <MONSTER/MonsterMode.h>
#include <MONSTER/MonsterUnique.h>
#include <PLAYER/PlayerPets.h>
#include <SKILLS/Skills.h>

// 1.13c D2Game+0x8CA20
void __fastcall sub_6FC3FC80(D2ClientStrc* pClient, D2UnitStrc* pUnit)
{
    if (!pUnit || pUnit->dwUnitType != UNIT_MONSTER)
    {
        return;
    }

    D2GSPacketSrvAC packetAC = {};
    packetAC.nHeader = 0xAC;
    packetAC.dwUnitGUID = pUnit->dwUnitId;
    packetAC.nUnitIndex = (uint16_t)pUnit->dwClassId;
    packetAC.nPosX = (uint16_t)CLIENTS_GetUnitX(pUnit);
    packetAC.nPosY = (uint16_t)CLIENTS_GetUnitY(pUnit);
    packetAC.nLifePct = sub_6FC62F50(pUnit);   // life in 128ths

    D2BitBufferStrc bitBuffer = {};
    BITMANIP_Initialize(&bitBuffer, packetAC.bitstream, sizeof(packetAC.bitstream));

    // Only the modes a client cannot derive are sent; everything else arrives as neutral.
    const uint32_t nMode = pUnit->dwAnimMode;
    BITMANIP_Write(&bitBuffer, (nMode == MONMODE_SKILL1 || nMode == MONMODE_SKILL2 || nMode == MONMODE_DEAD || nMode == MONMODE_DEATH) ? nMode : MONMODE_NEUTRAL, 4);

    if (MONSTER_HasComponents(pUnit))
    {
        BITMANIP_Write(&bitBuffer, 1, 1);
        if (pUnit->pMonsterData)
        {
            for (int32_t i = 0; i < 16; ++i)
            {
                const int32_t nCompInfo = D2COMMON_11068_GetCompInfo(pUnit, i);
                uint32_t nBits = 1;
                if (nCompInfo >= 3)
                {
                    nBits = 32 - (uint32_t)__builtin_clz((uint32_t)nCompInfo - 1);   // bsr + 1
                }
                BITMANIP_Write(&bitBuffer, pUnit->pMonsterData->nComponent[i], nBits);
            }
        }
    }
    else
    {
        BITMANIP_Write(&bitBuffer, 0, 1);
    }

    if (MONSTERUNIQUE_HasUMods(pUnit) || (pUnit->dwFlags & UNITFLAG_ISMERC))
    {
        BITMANIP_Write(&bitBuffer, 1, 1);
        BITMANIP_Write(&bitBuffer, MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_CHAMPION) != 0, 1);
        BITMANIP_Write(&bitBuffer, MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_UNIQUE) != 0, 1);
        BITMANIP_Write(&bitBuffer, MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_SUPERUNIQUE) != 0, 1);
        BITMANIP_Write(&bitBuffer, MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_MINION) != 0, 1);
        BITMANIP_Write(&bitBuffer, MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_GHOSTLY) != 0, 1);

        if (MONSTERUNIQUE_CheckMonTypeFlag(pUnit, MONTYPEFLAG_SUPERUNIQUE))
        {
            BITMANIP_Write(&bitBuffer, (uint32_t)MONSTERUNIQUE_GetBossHcIdx(pUnit), 16);
        }

        if (const uint8_t* pUMods = MONSTERUNIQUE_GetUMods(pUnit))
        {
            for (int32_t i = 0; i < 9 && pUMods[i]; ++i)
            {
                BITMANIP_Write(&bitBuffer, pUMods[i], 8);
            }
        }
        BITMANIP_Write(&bitBuffer, 0, 8);
        BITMANIP_Write(&bitBuffer, (uint32_t)MONSTERUNIQUE_GetNameSeed(pUnit), 16);

        D2UnitStrc* pOwner = AIGENERAL_GetMinionOwner(pUnit);
        if (pOwner && pOwner->dwUnitType == UNIT_PLAYER && PLAYERPETS_GetPetTypeFromPetGUID(pOwner, packetAC.dwUnitGUID) == PETTYPE_HIREABLE)
        {
            BITMANIP_Write(&bitBuffer, 1, 1);
            BITMANIP_Write(&bitBuffer, pOwner->dwUnitId, 32);
        }
        else
        {
            BITMANIP_Write(&bitBuffer, 0, 1);
        }
    }
    else
    {
        BITMANIP_Write(&bitBuffer, 0, 1);
    }

    if ((pUnit->dwFlagEx & UNITFLAGEX_STOREOWNERINFO) && pUnit->dwOwnerType == UNIT_PLAYER)
    {
        BITMANIP_Write(&bitBuffer, 1, 1);
        BITMANIP_Write(&bitBuffer, pUnit->dwOwnerGUID & 0x8FFFFFFF, 31);
    }
    else
    {
        BITMANIP_Write(&bitBuffer, 0, 1);
    }

    bool bStatsSent = false;
    if (D2StatListStrc* pStatList = STATLIST_GetStatListFromUnitStateAndFlag(pUnit, 0, 0x40))
    {
        D2StatStrc stats[16] = {};
        const int32_t nStatCount = STATLIST_GetBaseStatsData(pStatList, stats, 16);
        for (int32_t i = 0; i < nStatCount; ++i)
        {
            D2ItemStatCostTxt* pRecord = SKILLS_GetItemStatCostTxtRecord(stats[i].nStat);
            if (!pRecord || !(pRecord->dwItemStatFlags & gdwBitMasks[ITEMSTATCOSTFLAGINDEX_SEND_OTHER]) || !pRecord->nSendBits)
            {
                continue;
            }
            if (!bStatsSent)
            {
                bStatsSent = true;
                BITMANIP_Write(&bitBuffer, 1, 1);
            }
            BITMANIP_Write(&bitBuffer, stats[i].nStat, 9);
            if (pRecord->nSendParamBits)
            {
                BITMANIP_Write(&bitBuffer, stats[i].nLayer, pRecord->nSendParamBits);
            }
            BITMANIP_Write(&bitBuffer, (uint32_t)stats[i].nValue, pRecord->nSendBits);
        }
        BITMANIP_Write(&bitBuffer, bStatsSent ? 511 : 0, bStatsSent ? 9 : 1);
    }
    if (!bStatsSent)
    {
        BITMANIP_Write(&bitBuffer, 0, 1);
    }

    packetAC.nPacketLength = (uint8_t)(BITMANIP_GetSize(&bitBuffer) + 13);
    D2GAME_PACKETS_SendPacket_6FC3C710(pClient, &packetAC, packetAC.nPacketLength);
}
