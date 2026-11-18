#include <D2Game/GAME/Game2BB10Retail113.h>
#include <test_seam.h>
#include <Server.h>
#include "GAME/SCmd.h"

// The retail function reads its state from five module words: the game table (0x6FD31C24) is gpGameDataTbl_6FD45818,
// the callback block (0x6FD31C3C) is gpD2EventCallbackTable_6FD45830 (+0x08 = pfGetDatabaseCharacter, +0x20 =
// pfUnlockDatabaseCharacter), the act byte (0x6FD31C30) is gnAct_6FD45824; the slot table (0x6FD30C04) is read through
// GAME_GetGameGUIDFromGameId.
extern int32_t gnAct_6FD45824;
extern D2GameDataTableStrc* gpGameDataTbl_6FD45818;

// 1.13c D2Game+0x29BA0
void __fastcall GAME_UnlockGame_29BA0(D2GameStrc* pGame)
{
	D2_ASSERT(pGame);
	D2_UNLOCK(pGame->lpCriticalSection);   // asserts the section exists; the host build has no lock to release
}

// 1.13c D2Game+0x2AC60
void __fastcall GAME_SendGameInit_2AC60(D2ClientStrc* pClient, D2GameStrc* pGame)
{
	D2_CALL(D2GAME_PACKETS_SendPacket0x01_6FC3C7C0, pClient, 1, pGame);
	D2_CALL(D2GAME_PACKETS_SendHeaderOnlyPacket, pClient, 0);
	D2_ASSERT(pClient);
	D2_CALL(CLIENTS_SetClientState, pClient, CLIENTSTATE_GAME_INIT_SENT);
}

// 1.13c D2Game+0x49D40
void __fastcall CLIENTS_SetUnlockCharacter_49D40(D2ClientStrc* pClient)
{
	D2_ASSERT(pClient);
	pClient->bUnlockCharacter = 1;
}

// 1.13c D2Game+0x2BB10
void __fastcall GAME_JoinGame_2BB10(int32_t nClientId, uint16_t nGameId, int32_t nClass, char* szClientName, char* szAccountName, int32_t nCharSaveTransactionToken, int32_t nLocale, int32_t a8, int32_t a9)
{
	if (!gpGameDataTbl_6FD45818)
	{
		return;
	}

	const D2GameGUID nGUID = D2_CALL(GAME_GetGameGUIDFromGameId, nGameId);
	if (!nGUID)
	{
		return;
	}

	D2GameStrc* pGame = D2_CALL(GAME_LockGame, nGUID);
	if (!pGame)
	{
		return;
	}

	D2_CALL(SERVER_SetClientGameGUID, nClientId, nGUID);

	D2ClientStrc* pClient = D2_CALL(CLIENTS_AddToGame, pGame, nClientId, (uint8_t)nClass, szClientName, szAccountName, nCharSaveTransactionToken, (uint32_t)nLocale, a8, a9);
	if (!pClient)
	{
		if (gpD2EventCallbackTable_6FD45830 && gpD2EventCallbackTable_6FD45830->pfUnlockDatabaseCharacter)
		{
			gpD2EventCallbackTable_6FD45830->pfUnlockDatabaseCharacter(&pGame->nGameData, szClientName, szAccountName);
		}

		D2_CALL(GAME_UnlockGame_29BA0, pGame);
		D2_CALL(sub_6FC3C690, nClientId);
		D2_CALL(D2NET_10016, nClientId);
		return;
	}

	D2_CALL(GAME_SendGameInit_2AC60, pClient, pGame);
	pClient->nAct = (uint8_t)gnAct_6FD45824;
	D2_CALL(GAME_UnlockGame_29BA0, pGame);

	if (gpD2EventCallbackTable_6FD45830 && gpD2EventCallbackTable_6FD45830->pfGetDatabaseCharacter)
	{
		gpD2EventCallbackTable_6FD45830->pfGetDatabaseCharacter(&pClient->pClientInfo, szClientName, nClientId, szAccountName);
		D2_CALL(CLIENTS_SetUnlockCharacter_49D40, pClient);
		return;
	}

	D2_CALL(D2GAME_PACKETS_SendHeaderOnlyPacket, pClient, 2);
}
