#include "PLAYER/PartyScreen.h"
#include <test_seam.h>

#include <D2Dungeon.h>
#include <D2StatList.h>

#include "GAME/Clients.h"
#include "GAME/SCmd.h"
#include "PLAYER/Player.h"
#include "PLAYER/PlayerList.h"
#include "PLAYER/PlrTrade.h"
#include "UNIT/Friendly.h"
#include "UNIT/Party.h"
#include "UNIT/SUnit.h"


#pragma warning(disable: 28159)

enum D2PartyScreenType
{
    PARTYSCREENTYPE_ALLOW_MALE_TO_LOOT = 1,
    PARTYSCREENTYPE_ALLOW_FEMALE_TO_LOOT = 2,
    PARTYSCREENTYPE_HOSTILE = 3,
    PARTYSCREENTYPE_NO_LONGER_HOSTILE = 4,
    PARTYSCREENTYPE_RECEIVED_INVITE = 5,
    PARTYSCREENTYPE_INVITATION_CANCELLED = 6,
    PARTYSCREENTYPE_HAS_JOINED_PARTY = 7,
    PARTYSCREENTYPE_NOW_ALLIED_WITH = 8,
    PARTYSCREENTYPE_HAS_LEFT_PARTY = 9,
    PARTYSCREENTYPE_DISALLOW_MALE_TO_LOOT = 10,
    PARTYSCREENTYPE_DISALLOW_FEMALE_TO_LOOT = 11,
};


#pragma pack(push, 1)
struct D2PartyScreenCallbackStrc
{
    D2UnitStrc* pPlayer1;
    D2UnitStrc* pPlayer2;
    int16_t nPartyId;
};
#pragma pack(pop)


// Retail inlines the player->client lookup at every party-screen site: the player data's client
// for a player unit, null for anything else.
static D2ClientStrc* PARTYSCREEN_GetClient(D2UnitStrc* pPlayer)
{
    if (!pPlayer || pPlayer->dwUnitType != UNIT_PLAYER)
    {
        return nullptr;
    }

    return D2_CALL(UNITS_GetPlayerData, pPlayer)->pClient;
}

// 1.13c D2Game+0xF0C60: both roster records, the relation flag set to the requested state, and the
// roster update both ways. The lootability command is its only caller.
static void PARTYSCREEN_SetRosterFlag(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nFlag, int32_t nParam)
{
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer1, pPlayer2);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer2, pPlayer1);

    if (D2_CALL(PLAYERLIST_CheckFlag, pPlayer1, pPlayer2, nFlag) != nParam)
    {
        D2_CALL(PLAYERLIST_ToggleFlag, pGame, pPlayer1, pPlayer2, nFlag, nParam);
    }

    SCMD_Send0x75_PartyRosterUpdate(pPlayer1, pPlayer2);
    SCMD_Send0x75_PartyRosterUpdate(pPlayer2, pPlayer1);
}

//D2Game.0x6FC7AB50
// 1.13c D2Game+0xF0F40
void __fastcall PARTYSCREEN_ToggleLootability(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(pFailure);
    if (!CLIENTS_CheckFlag(PARTYSCREEN_GetClient(pPlayer1), CLIENTSAVEFLAG_HARDCORE))
    {
        return;
    }

    PARTYSCREEN_SetRosterFlag(pGame, pPlayer1, pPlayer2, 1, nParam);

    uint8_t nType = 0;
    if (!pPlayer1 || (pPlayer1->dwClassId > PCLASS_SORCERESS && pPlayer1->dwClassId != PCLASS_ASSASSIN))
    {
        nType = nParam != 0 ? PARTYSCREENTYPE_ALLOW_MALE_TO_LOOT : PARTYSCREENTYPE_DISALLOW_MALE_TO_LOOT;
    }
    else
    {
        nType = nParam != 0 ? PARTYSCREENTYPE_ALLOW_FEMALE_TO_LOOT : PARTYSCREENTYPE_DISALLOW_FEMALE_TO_LOOT;
    }

    PARTYSCREEN_SendEventUpdateToClient(PARTYSCREEN_GetClient(pPlayer2), pPlayer1 ? pPlayer1->dwUnitId : -1, nType);
}

//D2Game.0x6FC7AC20
// 1.13c D2Game+0xF0720
void __fastcall PARTYSCREEN_SendEventUpdateToClient(D2ClientStrc* pClient, int32_t nUnitGUID, uint8_t nType)
{
    D2GSPacketSrv5A packet5A = {};
    packet5A.nHeader = 0x5A;
    packet5A.nType = EVENTTYPE_PVPSTATUS;
    packet5A.nParam = nType;
    packet5A.dwParam = nUnitGUID;

    uint8_t nColor = STRCOLOR_BLUE;
    switch (nType)
    {
    case PARTYSCREENTYPE_ALLOW_MALE_TO_LOOT:
    case PARTYSCREENTYPE_ALLOW_FEMALE_TO_LOOT:
        nColor = STRCOLOR_DARK_GOLD;
        break;
    case PARTYSCREENTYPE_HOSTILE:
        nColor = STRCOLOR_ORANGE;
        break;
    case PARTYSCREENTYPE_NO_LONGER_HOSTILE:
        nColor = STRCOLOR_WHITE;
        break;
    case PARTYSCREENTYPE_RECEIVED_INVITE:
    case PARTYSCREENTYPE_HAS_JOINED_PARTY:
    case PARTYSCREENTYPE_NOW_ALLIED_WITH:
        nColor = STRCOLOR_LIGHTGREEN;
        break;
    case PARTYSCREENTYPE_INVITATION_CANCELLED:
    case PARTYSCREENTYPE_HAS_LEFT_PARTY:
    case PARTYSCREENTYPE_DISALLOW_MALE_TO_LOOT:
    case PARTYSCREENTYPE_DISALLOW_FEMALE_TO_LOOT:
        nColor = STRCOLOR_YELLOW;
        break;
    default:
        break;
    }
    packet5A.nColor = nColor;

    D2GAME_PACKETS_SendPacket0x5A_6FC3DEC0(pClient, &packet5A);
}

//D2Game.0x6FC7AD10
// 1.13c D2Game+0xF0EE0
void __fastcall PARTYSCREEN_ToggleIgnore(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(pFailure);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer1, pPlayer2);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer2, pPlayer1);

    if ((uint32_t)PLAYERLIST_CheckFlag(pPlayer1, pPlayer2, 2) != (uint32_t)nParam)
    {
        PLAYERLIST_ToggleFlag(pGame, pPlayer1, pPlayer2, 2, nParam);
    }

    SCMD_Send0x75_PartyRosterUpdate(pPlayer1, pPlayer2);
    SCMD_Send0x75_PartyRosterUpdate(pPlayer2, pPlayer1);
}

//D2Game.0x6FC7AD70
// 1.13c D2Game+0xF0E80
void __fastcall PARTYSCREEN_ToggleSquelch(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(pFailure);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer1, pPlayer2);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer2, pPlayer1);

    if ((uint32_t)PLAYERLIST_CheckFlag(pPlayer1, pPlayer2, 4) != (uint32_t)nParam)
    {
        PLAYERLIST_ToggleFlag(pGame, pPlayer1, pPlayer2, 4, nParam);
    }

    SCMD_Send0x75_PartyRosterUpdate(pPlayer1, pPlayer2);
    SCMD_Send0x75_PartyRosterUpdate(pPlayer2, pPlayer1);
}

//D2Game.0x6FC7ADD0
// 1.13c D2Game+0xF0D50
void __fastcall D2GAME_PARTYSCREEN_Command8_6FC7ADD0(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(nParam);
    D2_MAYBE_UNUSED(pFailure);
    uint16_t nPartyId = (uint16_t)SUNIT_GetPartyId(pPlayer2);

    PLAYERLIST_AllocPlayerList(pGame, pPlayer1, pPlayer2);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer2, pPlayer1);

    if ((int16_t)sub_6FCBA9D0(pPlayer1, pPlayer2) != 2)
    {
        sub_6FCBAA20(pPlayer2, pPlayer1);
        sub_6FCBAA20(pPlayer1, pPlayer2);
        return;
    }

    if (nPartyId == 0xFFFF)
    {
        nPartyId = (uint16_t)sub_6FCB9C40(pGame);
        sub_6FCB9D10(pGame, nPartyId, pPlayer2);
        sub_6FCB9D10(pGame, nPartyId, pPlayer1);
        PLAYERLIST_sub_6FCBAD60(pGame, pPlayer2);
    }
    else
    {
        sub_6FCB9D10(pGame, nPartyId, pPlayer1);
    }

    PLAYERLIST_sub_6FCBAD60(pGame, pPlayer1);

    PARTYSCREEN_SendEventUpdateToClient(PARTYSCREEN_GetClient(pPlayer1), pPlayer2 ? pPlayer2->dwUnitId : -1, PARTYSCREENTYPE_NOW_ALLIED_WITH);

    D2PartyScreenCallbackStrc arg = {};
    arg.pPlayer1 = pPlayer2;
    arg.pPlayer2 = pPlayer1;
    arg.nPartyId = (int16_t)nPartyId;
    SUNIT_IterateLivingPlayers(pGame, sub_6FC7AED0, &arg);
}

//Inlined in D2Game.0x6FC7AED0
// 1.13c D2Game+0xF09E0
void __fastcall inlinedfunction(D2UnitStrc* pLocalPlayer, D2UnitStrc* pOtherPlayer)
{
    const int32_t nLocalGUID = pLocalPlayer ? pLocalPlayer->dwUnitId : -1;
    const int32_t nOtherGUID = pOtherPlayer ? pOtherPlayer->dwUnitId : -1;
    if (nLocalGUID == nOtherGUID)
    {
        return;
    }

    const int16_t nPartyId = SUNIT_GetPartyId(pLocalPlayer);
    if (nPartyId == -1 && D2_CALL(sub_6FCBA9D0, pOtherPlayer, pLocalPlayer) == 4)
    {
        D2_CALL(sub_6FCBA980, pLocalPlayer, pOtherPlayer, 2);
        return;
    }

    D2_CALL(sub_6FCBA980, pOtherPlayer, pLocalPlayer, nPartyId != -1);
    D2_CALL(sub_6FCBA980, pLocalPlayer, pOtherPlayer, 1);
}

//D2Game.0x6FC7AED0
// 1.13c D2Game+0xF0B90
void __fastcall sub_6FC7AED0(D2GameStrc* pGame, D2UnitStrc* pLocalPlayer, void* a3)
{
    D2PartyScreenCallbackStrc* pArg = (D2PartyScreenCallbackStrc*)a3;

    const int16_t nPartyId = SUNIT_GetPartyId(pLocalPlayer);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pLocalPlayer, pArg->pPlayer1);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pArg->pPlayer1, pLocalPlayer);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pLocalPlayer, pArg->pPlayer2);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pArg->pPlayer2, pLocalPlayer);

    inlinedfunction(pLocalPlayer, pArg->pPlayer1);
    inlinedfunction(pLocalPlayer, pArg->pPlayer2);

    if (nPartyId == pArg->nPartyId && nPartyId != -1 && pLocalPlayer != pArg->pPlayer2)
    {
        D2_CALL(PARTYSCREEN_SendEventUpdateToClient, PARTYSCREEN_GetClient(pLocalPlayer), pArg->pPlayer2 ? pArg->pPlayer2->dwUnitId : -1, PARTYSCREENTYPE_HAS_JOINED_PARTY);
    }

    D2_CALL(sub_6FCBAA20, pLocalPlayer, pArg->pPlayer1);
    D2_CALL(sub_6FCBAA20, pLocalPlayer, pArg->pPlayer2);
    sub_6FC3F8F0(pGame, pLocalPlayer);
}

//D2Game.0x6FC7B030
// 1.13c D2Game+0xF0900
void __fastcall PARTYSCREEN_InvitationCancelled(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(nParam);
    D2_MAYBE_UNUSED(pFailure);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer1, pPlayer2);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer2, pPlayer1);

    if ((int16_t)D2_CALL(sub_6FCBA9D0, pPlayer1, pPlayer2) == 4)
    {
        D2_CALL(sub_6FCBA980, pPlayer1, pPlayer2, SUNIT_GetPartyId(pPlayer2) != -1);
        D2_CALL(sub_6FCBA980, pPlayer2, pPlayer1, SUNIT_GetPartyId(pPlayer1) != -1);
        D2_CALL(PARTYSCREEN_SendEventUpdateToClient, PARTYSCREEN_GetClient(pPlayer2), pPlayer1 ? pPlayer1->dwUnitId : -1, PARTYSCREENTYPE_INVITATION_CANCELLED);
    }

    D2_CALL(sub_6FCBAA20, pPlayer1, pPlayer2);
    D2_CALL(sub_6FCBAA20, pPlayer2, pPlayer1);
}

//D2Game.0x6FC7B0E0
// 1.13c D2Game+0xF0870
void __fastcall PARTYSCREEN_ReceivedInvitation(D2GameStrc* pGame, D2UnitStrc* pPlayer2, D2UnitStrc* pPlayer1, int32_t nParam, int32_t* pFailure)
{
    // 1.13c: the first unit argument (EDX) is the one that receives the invite, the second (stack) the sender; the body below reads them by role
    D2_MAYBE_UNUSED(nParam);
    D2_MAYBE_UNUSED(pFailure);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer1, pPlayer2);
    D2_CALL(PLAYERLIST_AllocPlayerList, pGame, pPlayer2, pPlayer1);

    if ((int16_t)D2_CALL(sub_6FCBA9D0, pPlayer1, pPlayer2) == 0)
    {
        D2_CALL(sub_6FCBA980, pPlayer1, pPlayer2, 4);
        D2_CALL(sub_6FCBA980, pPlayer2, pPlayer1, 2);
        D2_CALL(PARTYSCREEN_SendEventUpdateToClient, PARTYSCREEN_GetClient(pPlayer2), pPlayer1 ? pPlayer1->dwUnitId : -1, PARTYSCREENTYPE_RECEIVED_INVITE);
    }

    D2_CALL(sub_6FCBAA20, pPlayer1, pPlayer2);
    D2_CALL(sub_6FCBAA20, pPlayer2, pPlayer1);
}

//D2Game.0x6FC7B160
// 1.13c D2Game+0xF10F0
void __fastcall PARTYSCREEN_PlayerLeftParty(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2_MAYBE_UNUSED(pPlayer2);
    D2_MAYBE_UNUSED(nParam);
    D2_MAYBE_UNUSED(pFailure);
    const int16_t nPartyId = SUNIT_GetPartyId(pPlayer1);
    D2_CALL(PARTY_IteratePartyMembers, pGame, nPartyId, PARTYSCREEN_PartyIterate_PlayerLeftParty, pPlayer1);
    D2_CALL(PARTY_LeaveParty, pGame, pPlayer1);
}

//D2Game.0x6FC7B190
// 1.13c D2Game+0xF0820
void __fastcall PARTYSCREEN_PartyIterate_PlayerLeftParty(D2GameStrc* pGame, D2UnitStrc* pPlayer1, void* pArg)
{
    D2_MAYBE_UNUSED(pGame);
    D2UnitStrc* pPlayer2 = (D2UnitStrc*)pArg;
    if (pPlayer1 == pPlayer2)
    {
        return;
    }

    D2_CALL(PARTYSCREEN_SendEventUpdateToClient, PARTYSCREEN_GetClient(pPlayer1), pPlayer2 ? pPlayer2->dwUnitId : -1, PARTYSCREENTYPE_HAS_LEFT_PARTY);
}

// 1.13c D2Game+0xF0FF0: hostility opened by pPlayer1 against pPlayer2. A partied target turns the
// whole party hostile (when pPlayer1 is past level 8); a lone target gets hostility both ways
// (FRIENDLY_OpenHostility, which retail inlines here) and the HOSTILE event.
static void PARTYSCREEN_OpenHostility(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2)
{
    if (pPlayer2 && pPlayer2->pGame && pPlayer2->dwUnitType == UNIT_PLAYER)
    {
        const int16_t nPartyId = SUNIT_GetPartyId(pPlayer2);
        if (nPartyId != -1)
        {
            if (STATLIST_GetUnitBaseStat(pPlayer1, STAT_LEVEL, 0) > 8)
            {
                PARTY_IteratePartyMembers(pGame, nPartyId, sub_6FC7B3F0, pPlayer1);
            }
            return;
        }
    }

    FRIENDLY_OpenHostility(pGame, pPlayer1, pPlayer2);
    FRIENDLY_OpenHostility(pGame, pPlayer2, pPlayer1);
    PARTYSCREEN_SendEventUpdateToClient(PARTYSCREEN_GetClient(pPlayer2), pPlayer1 ? pPlayer1->dwUnitId : -1, PARTYSCREENTYPE_HOSTILE);
}

//D2Game.0x6FC7B1D0
// 1.13c D2Game+0xF1130
void __fastcall PARTYSCREEN_ToggleHostile(D2GameStrc* pGame, D2UnitStrc* pPlayer1, D2UnitStrc* pPlayer2, int32_t nParam, int32_t* pFailure)
{
    D2ActiveRoomStrc* pRoom = UNITS_GetRoom(pPlayer1);
    if (!pRoom || !DUNGEON_IsRoomInTown(pRoom) || STATLIST_GetUnitBaseStat(pPlayer1, STAT_LEVEL, 0) < 9 || STATLIST_GetUnitBaseStat(pPlayer2, STAT_LEVEL, 0) < 9)
    {
        *pFailure = 1;
        return;
    }

    PLAYERLIST_AllocPlayerList(pGame, pPlayer1, pPlayer2);
    PLAYERLIST_AllocPlayerList(pGame, pPlayer2, pPlayer1);

    if ((uint32_t)PLAYERLIST_CheckFlag(pPlayer1, pPlayer2, 8) != (uint32_t)nParam)
    {
        if (!PLAYERLIST_CheckFlag(pPlayer1, pPlayer2, 8) && STATLIST_UnitGetStatValue(pPlayer2, STAT_LEVEL, 0) < 9)
        {
            return;
        }

        if (!nParam)
        {
            FRIENDLY_RemoveHostility(pGame, pPlayer1, pPlayer2);
            PARTYSCREEN_SendEventUpdateToClient(PARTYSCREEN_GetClient(pPlayer2), pPlayer1 ? pPlayer1->dwUnitId : -1, PARTYSCREENTYPE_NO_LONGER_HOSTILE);
            return;
        }

        if (PLAYERLIST_GetHostileDelay(pPlayer1, pPlayer2) <= GetTickCount())
        {
            PLAYERLIST_SetHostileDelay(pPlayer1, pPlayer2, GetTickCount() + 60000);
            PARTYSCREEN_OpenHostility(pGame, pPlayer1, pPlayer2);
            sub_6FC7C170(pGame, pPlayer1);

            const int16_t nPartyId1 = SUNIT_GetPartyId(pPlayer1);
            if (nPartyId1 != -1 && nPartyId1 == SUNIT_GetPartyId(pPlayer2))
            {
                PARTYSCREEN_PlayerLeftParty(pGame, pPlayer1, nullptr, 0, nullptr);
            }
            return;
        }

        SUNIT_AttachSound(pPlayer1, 0x13u, pPlayer1);
        PLRTRADE_SendEventPacketToPlayer(pPlayer1, EVENTTYPE_PLEASEWAITHOSTILE, nullptr);
    }

    sub_6FCBAAA0(pGame, pPlayer1, pPlayer2);
    sub_6FCBAAA0(pGame, pPlayer2, pPlayer1);
}

//D2Game.0x6FC7B3F0
// 1.13c D2Game+0xF0CD0
void __fastcall sub_6FC7B3F0(D2GameStrc* pGame, D2UnitStrc* pPlayer1, void* pArg)
{
    D2UnitStrc* pPlayer2 = (D2UnitStrc*)pArg;
    if (pPlayer2 == pPlayer1 || D2_CALL(STATLIST_GetUnitBaseStat, pPlayer1, STAT_LEVEL, 0) <= 8)
    {
        return;
    }

    // 1.13c: the first worker call carries the unit, the second the other player (1.10f had them the other way round)
    D2_CALL(FRIENDLY_OpenHostility, pGame, pPlayer1, pPlayer2);
    D2_CALL(FRIENDLY_OpenHostility, pGame, pPlayer2, pPlayer1);
    D2_CALL(PARTYSCREEN_SendEventUpdateToClient, PARTYSCREEN_GetClient(pPlayer1), pPlayer2 ? pPlayer2->dwUnitId : -1, PARTYSCREENTYPE_HOSTILE);
}

//D2Game.0x6FC7B450
// not-1.13c: retail has no wrapper; the party functions at D2Game+0x9C76F and +0x9C8FB call the
// living-player walk (D2Game+0xDFC10) with D2Game+0xF0B10 themselves. Kept for UNIT/Party.cpp.
void __fastcall sub_6FC7B450(D2GameStrc* pGame, D2UnitStrc* pPlayer)
{
    SUNIT_IterateLivingPlayers(pGame, sub_6FC7B460, pPlayer);
}

//D2Game.0x6FC7B460
// 1.13c D2Game+0xF0B10
void __fastcall sub_6FC7B460(D2GameStrc* pGame, D2UnitStrc* pPlayer1, void* pArg)
{
    D2_MAYBE_UNUSED(pGame);
    D2UnitStrc* pPlayer2 = (D2UnitStrc*)pArg;
    if (pPlayer1 != pPlayer2)
    {
        D2_CALL(sub_6FCBA980, pPlayer1, pPlayer2, 0);
        D2_CALL(sub_6FCBA980, pPlayer2, pPlayer1, SUNIT_GetPartyId(pPlayer1) != -1);
        SCMD_Send0x75_PartyRosterUpdate(pPlayer1, pPlayer2);
        SCMD_Send0x75_PartyRosterUpdate(pPlayer2, pPlayer1);
        return;
    }

    D2_CALL(sub_6FCBA980, pPlayer1, pPlayer1, 1);
    SCMD_Send0x75_PartyRosterUpdate(pPlayer1, pPlayer1);
}

//D2Game.0x6FC7B4C0
// 1.13c D2Game+0xF0A70
int32_t __fastcall PARTYSCREEN_CommandHandler(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nCallbackId, int32_t nOtherPlayerGUID, int32_t nParam)
{
    constexpr void(__fastcall* gpfPartyCommands_6FD28E60[10])(D2GameStrc*, D2UnitStrc*, D2UnitStrc*, int32_t, int32_t*) =
    {
        nullptr,
        PARTYSCREEN_ToggleLootability,
        PARTYSCREEN_ToggleIgnore,
        PARTYSCREEN_ToggleSquelch,
        PARTYSCREEN_ToggleHostile,
        nullptr,
        PARTYSCREEN_ReceivedInvitation,
        PARTYSCREEN_InvitationCancelled,
        D2GAME_PARTYSCREEN_Command8_6FC7ADD0,
        PARTYSCREEN_PlayerLeftParty,
    };

    int32_t bFailure = 0;
    // Retail inlines SUNIT_GetServerUnit (pGame asserted, then the player hash chain).
    D2UnitStrc* pOtherPlayer = SUNIT_GetServerUnit(pGame, UNIT_PLAYER, nOtherPlayerGUID);
    if (!pOtherPlayer)
    {
        return 0;
    }

    if (gpfPartyCommands_6FD28E60[nCallbackId])
    {
        if (IsBadCodePtr((FARPROC)gpfPartyCommands_6FD28E60[nCallbackId]))
        {
            return 3;
        }

        gpfPartyCommands_6FD28E60[nCallbackId](pGame, pUnit, pOtherPlayer, nParam, &bFailure);
        if (bFailure)
        {
            return 3;
        }
    }

    return 0;
}
