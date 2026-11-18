#pragma once
// 1.13c D2Game functions moved from their stand-alone proof copies into the game tree (batch oc20-g11).
// Each is marked `// 1.13c D2Game+0x<RVA>` where it is defined.

#include <GAME/Clients.h>

// 1.13c D2Game+0x8B1F0 (eax = amount, ecx = side; stack = client): 6-byte packet 0x79
void __fastcall sub_8B1F0(D2ClientStrc* pClient, int32_t nAmount, char nSide);
// 1.13c D2Game+0x8B2F0 (eax = source; stack = client): relays a 0x15-byte block
void __fastcall sub_8B2F0(const void* pSrc, D2ClientStrc* pClient);

#include <GAME/Game.h>
// 1.13c D2Game+0x2A0D0 (esi = GUID): game.cpp, zeroes the GUID's slots in the game GUID table
void __fastcall GAME_RemoveGuidFromList(D2GameGUID nGameGUID);
extern int32_t gnGamesGUIDs_6FD447F8[1024];

#include <UNIT/Party.h>
// 1.13c D2Game+0x9BD00 (eax = game, cx = id): the party with the id, or null
D2PartyStrc* __fastcall sub_9BD00(D2GameStrc* pGame, int16_t nPartyId);
// 1.13c D2Game+0x9BD40 (eax = game): next free party id into the control's counter
void __fastcall sub_9BD40(D2GameStrc* pGame);
// 1.13c D2Game+0x9BF40 (eax = game, cx = id)
int32_t __fastcall PARTY_GetNodeCount(D2GameStrc* pGame, int16_t nPartyId);
// 1.13c D2Game+0x9C7D0 (eax = game, di = id)
void __fastcall PARTY_Delete(D2GameStrc* pGame, int16_t nPartyId);
// 1.13c D2Game+0x9C6F0 (oc20-g8, UNIT/Party.cpp): frees a party and its nodes
void __fastcall sub_6FCBC6F0(D2GameStrc* pGame, D2PartyStrc* pParty);
// 1.13c D2Game+0x9BC70 (eax = amount, ebx = unit, edi = out1, stack = out2)
void __fastcall sub_9BC70(D2UnitStrc* pUnit, int32_t* pOverflow, int32_t nAmount, int32_t* pAccepted);

#include <Units/Monster.h>
// 1.13c D2Game+0x8A330 (eax = node, edx = value): the shared tail of 8C910/8D500
int32_t __fastcall sub_8A330(D2UnkMonsterDataStrc* pNode, int32_t nValue);
// 1.13c D2Game+0x8C910 (eax = unit; stack = key unit, value, tag)
int32_t __fastcall sub_8C910(D2UnitStrc* pUnit, D2UnitStrc* pKeyUnit, int32_t nValue, int32_t nTag);
// 1.13c D2Game+0x8D500 (eax = unit, edi = value; stack = owner)
int32_t __fastcall sub_8D500(D2UnitStrc* pUnit, D2UnitStrc* pOwner, int32_t nValue);
// 1.13c D2Game+0x8E110 (ecx = object, eax = unit, edx = value; stack = tag; edi = the caller's value)
int32_t __fastcall sub_8E110(D2UnitStrc* pObject, D2UnitStrc* pUnit, int32_t nValue, int32_t nTag, int32_t nCallerValue);
#include <D2StatList.h>
#include <UNIT/SUnitDmg.h>
// 1.13c D2Game+0x91A30 (stdcall: unit, stat list, damage): case 3 of the progressive-damage switch 0x91E30
void __stdcall sub_91A30(D2UnitStrc* pUnit, D2StatListStrc* pStatList, D2DamageStrc* pDamage);
