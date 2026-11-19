#pragma once

#include <Units/Units.h>
#include <D2DataTbls.h>


//D2Game.0x6FC7B550
void __fastcall PLAYER_CreateIntros(D2GameStrc* pGame, D2UnitStrc* pPlayer);
void __fastcall PLAYER_Create(D2GameStrc* pGame, D2UnitStrc* pPlayer, int32_t nPlayerGUID);
// 1.13c D2Game+0x37500
void __fastcall PLAYER_FreeNodeChain(D2GameStrc* pGame, D2PlayerDataStrc* pPlayerData);
// 1.13c D2Game+0x376B0
void __fastcall PLAYER_FreeIntros(D2GameStrc* pGame, D2UnitStrc* pPlayer);
//D2Game.0x6FC7B630
void __fastcall PLAYER_Destroy(D2GameStrc* pGame, D2UnitStrc* pPlayer);
//D2Game.0x6FC7B750
void __fastcall PLAYER_RemoveAllPlayers(D2GameStrc* pGame);
//D2Game.0x6FC7B7A0
void __fastcall sub_6FC7B7A0(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nX, int32_t nY);
// 1.13c D2Game+0x38090
D2UnitStrc* __fastcall PLAYER_CreateStartItemUnit(D2UnitStrc* pPlayer, D2CharStatsTxt* pCharStatsTxt, D2GameStrc* pGame, int32_t nCount, int32_t nItemId);
//D2Game.0x6FC7B800
void __fastcall PLAYER_CreateStartItem(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2CharStatsTxt* pCharStatsTxt, int32_t nCount, D2CharItemStrc* pCharItem);
// 1.13c D2Game+0x37EC0
int32_t __fastcall PLAYER_PlaceStartItemInBelt(D2UnitStrc* pItem, D2UnitStrc* pPlayer, D2GameStrc* pGame, int32_t bFindFreeSlot);
// 1.13c D2Game+0x37F20
int32_t __fastcall PLAYER_PlaceStartItemOnBody(D2UnitStrc* pItem, D2UnitStrc* pPlayer, D2GameStrc* pGame);
//D2Game.0x6FC7BB50
void __fastcall PLAYER_PlaceItemInInventory(D2UnitStrc* pItem, D2UnitStrc* pPlayer, D2GameStrc* pGame, int32_t a8);
//D2Game.0x6FC7BC30
void __fastcall PLAYER_CreateStartItemsFromCharStatsTxt(D2UnitStrc* pPlayer, D2GameStrc* pGame, void* pUnused);
//D2Game.0x6FC7BC90
void __fastcall PLAYER_SynchronizeItemsToClient(D2UnitStrc* pUnit, D2ClientStrc* pClient);
//D2Game.0x6FC7BD50
int32_t __fastcall PLAYER_IsBusy(D2UnitStrc* pUnit);
//D2Game.0x6FC7BDB0
//D2Game.0x6FC7BDF0
void __fastcall PLAYER_StopInteractions(D2GameStrc* pGame, D2UnitStrc* pUnit);
//D2Game.0x6FC7BEC0
void __fastcall sub_6FC7BEC0(D2GameStrc* pGame, D2UnitStrc* pUnit);
//1.10f: D2Game.0x6FC7BFC0
//1.13c: D2Game.0x6FC57B10
void __fastcall sub_6FC7BFC0(D2GameStrc* pGame, D2ActiveRoomStrc* pRoom, int32_t nPlayerGUID, D2CoordStrc* pCoord);
//D2Game.0x6FC7C0C0
void __fastcall PLAYER_SetUniqueIdInPlayerData(D2UnitStrc* pPlayer, int32_t nUnitGUID);
//D2Game.0x6FC7C120
int32_t __fastcall PLAYER_GetUniqueIdFromPlayerData(D2UnitStrc* pPlayer);
//D2Game.0x6FC7C170
void __fastcall sub_6FC7C170(D2GameStrc* pGame, D2UnitStrc* pPlayer);
//D2Game.0x6FC7C260
int32_t __fastcall sub_6FC7C260(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nUnitGUID, uint32_t nValue);
//D2Game.0x6FC7C3A0
int32_t __fastcall sub_6FC7C3A0(D2GameStrc* pGame, D2UnitStrc* pPlayer, D2UnitStrc* a3);
//D2Game.0x6FC7C450
int32_t __fastcall sub_6FC7C450(D2GameStrc* pGame, D2UnitStrc* pPlayer, int32_t nHotkeyId, int16_t nSkillId, char a4, int32_t nFlags);
//D2Game.0x6FC7C490 (#10059)
void __fastcall PLAYER_SetVirtualPlayerCount(int32_t nPlayers);
//D2Game.0x6FC7C4A0
int32_t __fastcall PLAYER_GetPlayerCount(D2GameStrc* pGame);
//D2Game.0x6FC7C4E0
void __fastcall PLAYER_CountLivingPlayers(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pLivingPlayers);
// 1.13c D2Game+0x38180
void __fastcall PLAYER_ApplyDeathGoldPenalty(D2GameStrc* pGame, D2UnitStrc* pDefender, D2UnitStrc* pAttacker);
// 1.13c D2Game+0x37730
void __fastcall PLAYER_LoseExperience(D2GameStrc* pGame, D2UnitStrc* pDefender);
//D2Game.0x6FC7C500
void __fastcall PLAYER_ApplyDeathPenalty(D2GameStrc* pGame, D2UnitStrc* pDefender, D2UnitStrc* pAttacker);
//D2Game.0x6FC7C750
int32_t __fastcall D2GAME_IteratePlayers_6FC7C750(D2GameStrc* pGame, D2UnitStrc* pUnit);
//D2Game.0x6FC7C790
void __fastcall sub_6FC7C790(D2GameStrc* pGame, D2UnitStrc* pUnit, void* pArgs);
//D2Game.0x6FC7C7B0
D2UnitStrc* __fastcall sub_6FC7C7B0(D2UnitStrc* pUnit);
//D2Game.0x6FC7C900
void __fastcall sub_6FC7C900(D2UnitStrc* pAttacker, int32_t a2);
//D2Game.0x6FC7CA10
void __fastcall sub_6FC7CA10(D2UnitStrc* pTarget, int32_t a2);
//D2Game.0x6FC7CA70
void __fastcall D2GAME_SetStatOrResetGold_6FC7CA70(D2UnitStrc* pUnit, int32_t nStat, int32_t nValue);
