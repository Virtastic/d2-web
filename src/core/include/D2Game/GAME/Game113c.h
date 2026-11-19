#pragma once
// d2-web: 1.13c D2Game functions that have no 1.10f twin (or whose 1.10f twin is inlined / has other offsets).
// Each lives in its own source file next to its area and carries the `// 1.13c D2Game+0x<RVA>` marker.

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <D2DataTbls.h>
#include <Units/Units.h>
#include <D2BitManip.h>
#include "GAME/Game.h"

// 1.13c D2Game+0x19F0: the MonProp record of a monster class (MonStats row, then its wMonProp), or null when either id is out of range.
D2MonPropTxt* __fastcall MONSTER_GetMonPropTxtRecordFromClass(int32_t nClassId);
// 1.13c D2Game+0x2790: nonzero when the (monster) unit's MonType is nested under MonType `nIndex` in the nest table.
uint32_t __fastcall MONSTER_IsUnitMonTypeNestedIn(D2UnitStrc* pUnit, int32_t nIndex);
// 1.13c D2Game+0x1CD0: variable-width unsigned field: 0 + 8 bits, 10 + 16 bits or 11 + 32 bits.
void __fastcall GAME_WriteVariableWidthBits(D2BitBufferStrc* pBuffer, uint32_t nValue);
// 1.13c D2Game+0x26F0: the SEH-wrapped list constructor: when pNode is non-null it is built as an empty list taking pList's link offset.
void __fastcall D2GameDataTable_TSExplicitList_InplaceNew_Guarded(TSExplicitList<D2GameStrc, 0xDDDDDDDD>* pList, void* pNode);
// 1.13c D2Game+0x2813: the CRT __stricmp (this runtime only has the startup C locale).
int __cdecl D2GAME_CRT_stricmp_6FC22813(const char* pString1, const char* pString2);
// 1.13c D2Game+0x2A110: join-name check (letters, one inner apostrophe/hyphen/underscore, fewer than nMaxLen characters).
bool __fastcall GAME_IsClientNameValid(const char* szName, int32_t nMaxLen);
// The VS2003 CRT FILE as _sprintf fills it (native pointers; only the fields _output and _sprintf touch).
struct D2CrtFileStrc
{
	char* _ptr;
	int32_t _cnt;
	char* _base;
	int32_t _flag;
};
// 1.13c D2Game+0x2A6D: the CRT _sprintf; _output (0x43E0) and __flsbuf (0x4239) are host stand-ins, not 1.13c bodies.
int __cdecl D2GAME_CRT_sprintf_6FC22A6D(char* szDest, const char* szFormat, ...);
int __cdecl D2GAME_CRT_output_6FC243E0(D2CrtFileStrc* pFile, const char* szFormat, va_list vaArgs);
int __cdecl D2GAME_CRT_flsbuf_6FC24239(int nChar, D2CrtFileStrc* pFile);
// 1.13c D2Game+0x2AD0: the CRT _memmove (overlap-safe copy, returns the destination).
void* __cdecl D2GAME_CRT_memmove_6FC22AD0(void* pDest, const void* pSrc, size_t nSize);
// 1.13c D2Game+0x2E50 / +0x3520 / +0x391B: _strncpy, _strncmp, _tolower (startup C locale).
char* __cdecl D2GAME_CRT_strncpy_6FC22E50(char* dest, const char* source, size_t count);
int __cdecl D2GAME_CRT_strncmp_6FC23520(const char* s1, const char* s2, size_t maxCount);
int32_t D2GAME_CRT_tolower_6FC2391B(int32_t c);
// 1.13c D2Game+0x2250: unlink a 32-bit-layout hash record from its two link pairs (+4/+8, +0xC/+0x10).
void sub_6FC22250(uint32_t* pEntry);
// 1.13c D2Game+0x2E10: the CRT __strrev (in-place reverse, returns the string).
char* __cdecl D2GAME_CRT_strrev_6FC22E10(char* szString);
// 1.13c D2Game+0x2AC60: send the game init (0x01 flags packet, header-only 0) and set the client to GAME_INIT_SENT.
void __fastcall GAME_SendGameInitToClient(D2ClientStrc* pClient);
// 1.13c D2Game+0x9BD00: the party of the game's party list with this id, or null.
struct D2PartyStrc* __fastcall PARTY_GetPartyById(D2GameStrc* pGame, int16_t nPartyId);
// 1.13c D2Game+0x497C0: by-name client lookup (0 = found, the stored name copied to szOutName when given; 1 = absent).
int32_t __fastcall CLIENTS_GetClientByName(const char* szName, char* szOutName);
// 1.13c D2Game+0x9D8E0: report each monster unit's event timers of type 2, then type 3, to sub_6FC34890 (pFirstUnit = the game's unit chain head at +0x74).
void __fastcall GAME_ReportUnitEventTimers(D2GameStrc* pGame, D2UnitStrc* pFirstUnit);
