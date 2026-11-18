// 1.13c packet senders and life-update filters; the 1.10f SCmd.cpp is the prior art for names and layout.
#include "GAME/SCmd.h"

#include <climits>
#include <cstring>
#include <algorithm>

#include <D2Skills.h>
#include <D2StatList.h>
#include <Path/Path.h>
#include <Units/Player.h>
#include <Units/Monster.h>
#include "GAME/Clients.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"
#include "AI/AiGeneral.h"
#include "../../../compat/Fog.h"
#include "../../../compat/Storm.h"
#include "../../../compat/test_seam.h"

// 1.13c D2Game+0x8B5A0: the skill list packet 0x94, {0x94, count, unit id, then id word + level byte per skill}.
void __fastcall sub_6FC3DCA0(D2ClientStrc* pClient, D2UnitStrc* pUnit)
{
    D2_ASSERT(pUnit && pUnit->pSkills);
    D2_ASSERT(pClient);

    int32_t nCount = 0;
    for (D2SkillStrc* pSkill = D2_CALL(SKILLS_GetFirstSkillFromSkillList, pUnit->pSkills); pSkill; pSkill = D2_CALL(SKILLS_GetNextSkill, pSkill))
    {
        ++nCount;
    }
    D2_ASSERT(nCount < 256);

    const int32_t nSize = 6 + 3 * nCount;
    // Retail takes the pool from the client's game; a client without one (the headless drivers') allocates from no pool.
    void* pMemPool = pClient->pGame ? pClient->pGame->pMemoryPool : nullptr;
    uint8_t* pPacket = (uint8_t*)D2_ALLOC_POOL(pMemPool, nSize);
    pPacket[0] = 0x94;
    pPacket[1] = (uint8_t)nCount;
    std::memcpy(pPacket + 2, &pUnit->dwUnitId, 4);   // the unit argument's id, not the client player's

    uint8_t* pOut = pPacket + 6;
    for (D2SkillStrc* pSkill = D2_CALL(SKILLS_GetFirstSkillFromSkillList, pUnit->pSkills); pSkill; pSkill = D2_CALL(SKILLS_GetNextSkill, pSkill))
    {
        pOut[2] = (uint8_t)D2_CALL(SKILLS_GetSkillLevel, pUnit, pSkill, 0);
        const int32_t nSkillId = D2_CALL(SKILLS_GetSkillIdFromSkill, pSkill, __FILE__, __LINE__);
        pOut[0] = (uint8_t)nSkillId;
        pOut[1] = (uint8_t)(nSkillId >> 8);
        pOut += 3;
    }

    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, pPacket, nSize);
    D2_FREE_POOL(pMemPool, pPacket);
}

// 1.13c D2Game+0x8BD00: one attribute to the client in the narrowest of byte (0x1D), word (0x1E) and dword (0x1F) form.
// An attribute id of 0xFF or more is retail's 0x3CB fatal.
void __fastcall D2GAME_PACKETS_SendPacket0x1D_E_F_6FC3D480(D2ClientStrc* pClient, uint16_t nStatId, uint32_t nValue)
{
    D2_ASSERT(nStatId < 0xFF);
    uint8_t p[6] = { 0x1D, (uint8_t)nStatId };
    int32_t nSize = 3;
    if (nValue >= 0xFFFFu) { p[0] = 0x1F; std::memcpy(p + 2, &nValue, 4); nSize = 6; }
    else if (nValue >= 0xFFu) { p[0] = 0x1E; std::memcpy(p + 2, &nValue, 2); nSize = 4; }
    else { p[2] = (uint8_t)nValue; }
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, p, nSize);
}

// 1.13c D2Game+0x8BF20: 0x08, a room the client leaves: header, tile x (u16), tile y (u16), level (u8). Same layout as 0x07.
void __fastcall D2GAME_PACKETS_SendPacket0x08_6FC3D160(D2ClientStrc* pClient, uint8_t nAreaId, uint16_t nTileX, uint16_t nTileY)
{
    uint8_t packet[6] = {};
    packet[0] = 0x08;
    std::memcpy(packet + 1, &nTileX, 2);
    std::memcpy(packet + 3, &nTileY, 2);
    packet[5] = nAreaId;
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, packet, (int32_t)sizeof packet);
}

// 1.13c D2Game+0x8C630: 0x8C, eleven bytes {0x8C, id dword, id dword, relation word} to the unit's client.
// The client is the player data's only when the unit is a non-null player; any other unit sends to a null client, which the send drops.
void __fastcall D2GAME_PACKETS_SendPacket0x8C_6FC3F8B0(D2UnitStrc* pUnit, DWORD dwPlayerId1, DWORD dwPlayerId2, WORD wRelationState)
{
    uint8_t b[11] = { 0x8C };
    std::memcpy(b + 1, &dwPlayerId1, 4);
    std::memcpy(b + 5, &dwPlayerId2, 4);
    std::memcpy(b + 9, &wRelationState, 2);
    D2ClientStrc* pClient = nullptr;
    if (pUnit && pUnit->dwUnitType == UNIT_PLAYER) pClient = UNITS_GetPlayerData(pUnit)->pClient;
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, b, (int32_t)sizeof b);
}

// 1.13c D2Game+0x8D710: 0x59, a player assigned to a client: header, unit id, class, name, x, y (26 bytes); then the roster line (0x75).
void __fastcall sub_6FC3D1F0(D2ClientStrc* pClient, int32_t nUnitGUID, uint8_t nClassId, const char* szName, int16_t nX, int16_t nY)
{
    D2GSPacketSrv59 packet59 = {};
    packet59.nHeader = 0x59u;
    packet59.dwGUID = nUnitGUID;
    packet59.nClass = nClassId;
    D2_CALL(SStrCopy, packet59.szName, szName, (size_t)INT_MAX);
    packet59.nPosX = nX;
    packet59.nPosY = nY;
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, &packet59, (int32_t)sizeof packet59);

    D2_ASSERT(pClient);   // 0xFB
    // The client's current unit: flag bit 0 keeps the cached one, otherwise it is resolved again by id and dropped when gone.
    D2UnitStrc* pLocalPlayer = pClient->pPlayer;
    if (!(pClient->dwFlags & 1) && pClient->pPlayer)
    {
        pLocalPlayer = D2_CALL(SUNIT_GetServerUnit, pClient->pGame, (int32_t)pClient->dwUnitType, (int32_t)pClient->dwUnitGUID);
        if (!pLocalPlayer)
        {
            pClient->pPlayer = nullptr;
        }
    }

    D2_ASSERT(pClient->pGame);   // 0x2D3
    D2UnitStrc* pOtherPlayer = D2_CALL(SUNIT_GetServerUnit, pClient->pGame, UNIT_PLAYER, nUnitGUID);
    if (pOtherPlayer)
    {
        D2_CALL(SCMD_Send0x75_PartyRosterUpdate, pLocalPlayer, pOtherPlayer);
    }
}

// The path-dependent head shared by the 0x68 senders: the unit's position (static paths hold it inline).
// Retail inlines this in each sender.
static void SCMD_FillUnitPosition(uint8_t* pPacket, D2UnitStrc* pUnit)
{
    const int32_t nType = pUnit->dwUnitType;
    const bool bStaticPath = nType == UNIT_OBJECT || (nType > UNIT_MISSILE && nType <= UNIT_TILE);
    int16_t nX, nY;
    if (bStaticPath)
    {
        nX = (int16_t)pUnit->pStaticPath->tGameCoords.nX;
        nY = (int16_t)pUnit->pStaticPath->tGameCoords.nY;
    }
    else
    {
        nX = (int16_t)(pUnit->pDynamicPath ? D2_CALL(PATH_GetXPosition, pUnit->pDynamicPath) : 0);
        nY = (int16_t)(pUnit->pDynamicPath ? D2_CALL(PATH_GetYPosition, pUnit->pDynamicPath) : 0);
    }
    std::memcpy(pPacket + 6, &nX, 2);
    std::memcpy(pPacket + 8, &nY, 2);
}

// The velocity stat clamped to a word, at 0x12.
static void SCMD_FillUnitVelocity(uint8_t* pPacket, D2UnitStrc* pUnit)
{
    int32_t nVelocity = D2_CALL(STATLIST_UnitGetStatValue, pUnit, STAT_VELOCITYPERCENT, 0);
    nVelocity = std::clamp(nVelocity, -0x8000, 0x7FFF);
    const int16_t nWord = (int16_t)nVelocity;
    std::memcpy(pPacket + 0x12, &nWord, 2);
}

// 1.13c D2Game+0x8D990: the 21-byte unit packet with an explicit life, hit class and max distance.
void __fastcall D2GAME_PACKETS_SendPacket0x68_6FC3CCB0(D2ClientStrc* pClient, uint8_t nHeader, int32_t nUnitGUID, int8_t a4, int8_t a5, int32_t a6, int8_t a7, int8_t a8, int8_t a9)
{
    D2_ASSERT(pClient);        // 0x996
    D2_ASSERT(pClient->pGame); // 0x2D3

    uint8_t packet[21] = {};
    packet[0] = nHeader;
    std::memcpy(packet + 1, &nUnitGUID, 4);
    packet[5] = (uint8_t)a4;
    packet[0xA] = (uint8_t)a5;
    std::memcpy(packet + 0xB, &a6, 4);

    D2UnitStrc* pUnit = D2_CALL(SUNIT_GetServerUnit, pClient->pGame, UNIT_MONSTER, nUnitGUID);
    if (!pUnit)
    {
        return;
    }

    SCMD_FillUnitPosition(packet, pUnit);
    packet[0xF] = (uint8_t)a7;
    packet[0x10] = (uint8_t)a9;
    uint8_t nMode = (uint8_t)D2_CALL(PATH_GetType, pUnit->pDynamicPath);
    if (nMode == 8)
    {
        nMode = 0xB;
    }
    packet[0x11] = nMode;
    SCMD_FillUnitVelocity(packet, pUnit);
    packet[0x14] = (uint8_t)a8;
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, packet, (int32_t)sizeof packet);
}

// 1.13c D2Game+0x8DC30: the same packet with the path's own max distance. Retail faults on a unit id it cannot find; here nothing is sent.
void __fastcall D2GAME_PACKETS_SendPacket0x68_6FC3CA90(D2ClientStrc* pClient, uint8_t nHeader, int32_t nUnitGUID, int8_t a4, int8_t a5, int32_t a6, int8_t a7)
{
    D2_ASSERT(pClient);        // 0x996
    D2_ASSERT(pClient->pGame); // 0x2D3

    uint8_t packet[21] = {};
    packet[0] = nHeader;
    std::memcpy(packet + 1, &nUnitGUID, 4);
    packet[5] = (uint8_t)a4;
    packet[0xA] = (uint8_t)a5;
    std::memcpy(packet + 0xB, &a6, 4);

    D2UnitStrc* pUnit = D2_CALL(SUNIT_GetServerUnit, pClient->pGame, UNIT_MONSTER, nUnitGUID);
    if (!pUnit)
    {
        return;
    }

    SCMD_FillUnitPosition(packet, pUnit);
    packet[0xF] = (uint8_t)a7;
    const uint8_t nMode = (uint8_t)D2_CALL(PATH_GetType, pUnit->pDynamicPath);
    packet[0x11] = nMode;
    if (nMode >= 5)
    {
        if (nMode <= 6) packet[0x11] = 2;
        else if (nMode == 8) packet[0x11] = 0xB;
    }
    SCMD_FillUnitVelocity(packet, pUnit);
    packet[0x14] = D2_CALL(PATH_GetMaxDistance, pUnit->pDynamicPath);
    D2_CALL(D2GAME_PACKETS_SendPacket_6FC3C710, pClient, packet, (int32_t)sizeof packet);
}

// 1.13c D2Game+0x8A330: true when the life percentage moved by more than 8, or by anything on more than 6 calls.
int32_t __stdcall sub_6FC3E3F0(D2UnkMonsterDataStrc* pState, int32_t nCurrentLifePercentage)
{
    int32_t nDiff = nCurrentLifePercentage - pState->nLifePercentage;
    if (nDiff == 0)
    {
        return 0;
    }
    if (nDiff < 0)
    {
        nDiff = -nDiff;
    }
    if (nDiff > 8 || ++pState->nCount > 6u)
    {
        pState->nCount = 0;
        pState->nLifePercentage = nCurrentLifePercentage;
        return 1;
    }
    return 0;
}

// 1.13c D2Game+0x8C910: the party member's life update filter, kept per recipient in its player data list keyed by the target's id.
// Unlike 1.10f the node's second word (a4) replaces the level id: a changed a4 resets the node and always sends.
int32_t __fastcall sub_6FC3E440(D2UnitStrc* pUnit1, D2UnitStrc* pUnit2, int32_t a3, int32_t a4)
{
    D2_ASSERT(pUnit1);
    D2GameStrc* pGame = pUnit1->pGame;
    if (!pGame)
    {
        return 0;
    }

    const int16_t nPartyId = D2_CALL(PARTY_GetPartyIdForUnitOwner, pGame, pUnit1);
    if (nPartyId == -1 || nPartyId != D2_CALL(PARTY_GetPartyIdForUnitOwner, pGame, pUnit2))
    {
        return 0;
    }

    D2PlayerDataStrc* pPlayerData = D2_CALL(UNITS_GetPlayerData, pUnit1);
    if (!pPlayerData)
    {
        return 0;
    }

    const int32_t nUnitGUID = pUnit2 ? (int32_t)pUnit2->dwUnitId : -1;
    D2UnkPlayerDataStrc* pNode = pPlayerData->unk0x98;
    while (pNode && pNode->nUnitGUID != nUnitGUID)
    {
        pNode = pNode->pNext;
    }

    if (!pNode)
    {
        pNode = (D2UnkPlayerDataStrc*)FOG_AllocPool(pGame->pMemoryPool, sizeof(D2UnkPlayerDataStrc), __FILE__, 0x6FA, 0);
        if (!pNode)
        {
            return 0;
        }
        std::memset(pNode, 0x00, sizeof(*pNode));
        pNode->pNext = pPlayerData->unk0x98;
        pPlayerData->unk0x98 = pNode;
        pNode->nUnitGUID = nUnitGUID;
    }

    if (pNode->unk0x08 != a4)
    {
        pNode->unk0x04 = a3;
        pNode->unk0x08 = a4;
        return 1;
    }

    return D2_CALL(sub_6FC3E3F0, (D2UnkMonsterDataStrc*)pNode, a3);
}

// 1.13c D2Game+0x8D500: the monster life filter. Sends only when the monster's owner (resolved from its AI control block)
// is pPlayer and the life percentage differs from the one last sent.
int32_t __fastcall SCMD_ShouldSendMonsterLifeUpdate(D2UnitStrc* pUnit, D2UnitStrc* pPlayer, int32_t nLifePercentage)
{
    D2UnitStrc* pOwner = nullptr;
    if (pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData && pUnit->pMonsterData->pAiControl && pUnit->pMonsterData->pAiControl->pGame)
    {
        D2AiControlStrc* pAiControl = pUnit->pMonsterData->pAiControl;
        pOwner = D2_CALL(SUNIT_GetServerUnit, pAiControl->pGame, (int32_t)pAiControl->dwOwnerTypeEx, (int32_t)pAiControl->dwOwnerGUIDEx);
    }

    if (pOwner == pPlayer && pUnit && pUnit->dwUnitType == UNIT_MONSTER && pUnit->pMonsterData
        && pUnit->pMonsterData->unk0x3C.nLifePercentage != nLifePercentage)
    {
        return D2_CALL(sub_6FC3E3F0, &pUnit->pMonsterData->unk0x3C, nLifePercentage);
    }
    return 0;
}

// 1.13c D2Game+0x8E110: the life update filter by unit type. pObject is the receiving player; pUnit the unit whose life moved.
int32_t __fastcall SCMD_ShouldSendUnitLifeUpdate(D2UnitStrc* pObject, D2UnitStrc* pUnit, int32_t a3, int32_t a4)
{
    D2_ASSERT(pObject);   // 0x712
    D2_ASSERT(pUnit);     // 0x713
    if (pUnit->dwUnitType == UNIT_PLAYER)
    {
        return D2_CALL(sub_6FC3E440, pObject, pUnit, a3, a4);
    }
    if (pUnit->dwUnitType == UNIT_MONSTER)
    {
        return D2_CALL(SCMD_ShouldSendMonsterLifeUpdate, pUnit, pObject, a3);
    }
    return 0;
}
