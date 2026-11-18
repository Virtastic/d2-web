// 1.13c D2Game+0x2BB10 (the join-game step of the packet pump 0xCAF30), written in the call shape of the retail function.
// The vendored 1.10f GAME_JoinGame (Game.cpp) is the same rule with the logging and the inline packet sends of 1.10f; here
// each retail callee is its own D2_CALL seam: the game-list slot read (GAME_GetGameGUIDFromGameId), the lock (0x2AAE0),
// the client bind (Ordinal 10021), the client add (0x4A9B0), the unlock (0x29BA0), the game-init send (0x2AC60), the
// client mark (0x49D40) and the two Ordinal calls of the no-client branch (0xB0 packet, 10000).
#pragma once
#include "GAME/Game.h"
#include "GAME/Clients.h"

//D2Game.0x6FC49BA0
void __fastcall GAME_UnlockGame_29BA0(D2GameStrc* pGame);
//D2Game.0x6FC4AC60: game-init packet (0x01), header-only packet 0, then the client state GAME_INIT_SENT
void __fastcall GAME_SendGameInit_2AC60(D2ClientStrc* pClient, D2GameStrc* pGame);
//D2Game.0x6FC69D40
void __fastcall CLIENTS_SetUnlockCharacter_49D40(D2ClientStrc* pClient);
//D2Game.0x6FC4BB10
void __fastcall GAME_JoinGame_2BB10(int32_t nClientId, uint16_t nGameId, int32_t nClass, char* szClientName, char* szAccountName, int32_t nCharSaveTransactionToken, int32_t nLocale, int32_t a8, int32_t a9);
