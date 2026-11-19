#include "GAME/CCmd.h"
#include <test_seam.h>
#include <Server.h>
#include <Fog.h>
#include "GAME/Game.h"
#include "GAME/Clients.h"
#include "PLAYER/PlrMsg.h"

// 1.13c D2Game+0xCAE80
// Per-client game packet entry. The first dword of pData is the client id, the packet follows it.
// The client must still be in the game's client list; otherwise the game is just unlocked.
void __fastcall CCMD_ProcessClientMessage(void* pData, int32_t nPacketSize)
{
	if (D2Seam<&CCMD_ProcessClientMessage>::hook) { D2Seam<&CCMD_ProcessClientMessage>::hook(pData, nPacketSize); return; }
    const int32_t nClientId = *(int32_t*)pData;
    D2GameStrc* pGame = D2_CALL(GAME_LockGame, (D2GameGUID)D2_CALL(SERVER_GetClientGameGUID, nClientId));
    if (!pGame)
    {
        return;
    }

    D2ClientStrc* pListed = pGame->pClientList;
    if (pListed)
    {
        while (pListed->dwClientId != (uint32_t)nClientId)
        {
            pListed = pListed->pNext;
            if (!pListed)
            {
                D2_CALL(GAME_LeaveGamesCriticalSection, pGame);
                return;
            }
        }

        D2ClientStrc* pClient = D2_CALL(CLIENTS_GetClientFromClientId, pGame, nClientId);
        D2_ASSERT(pClient);
        pClient->dwLastPacketTick = D2_CALL(GetTickCount);

        D2UnitStrc* pPlayer = D2_CALL(CLIENTS_GetPlayerFromClient, pClient, 0);
        if (pPlayer && pPlayer->dwUnitType == UNIT_PLAYER)
        {
            D2_CALL(D2GAME_PACKET_Handler_6FC89320, pGame, pPlayer, (uint8_t*)pData + 4, nPacketSize);
        }
    }

    D2_CALL(GAME_LeaveGamesCriticalSection, pGame);
}
