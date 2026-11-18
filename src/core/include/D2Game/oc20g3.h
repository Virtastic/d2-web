#pragma once
// 1.13c D2Game functions moved from their stand-alone proof copies into the game tree (batch oc20-g3).
// Each is marked `// 1.13c D2Game+0x<RVA>` where it is defined; the sources sit beside the 1.10f source file of their area.

#include <Units/Units.h>

// 1.13c D2Game+0xD5D0
int32_t __fastcall sub_D5D0(D2GameStrc* pGame, D2UnitStrc* pUnit, int32_t nMissile, int32_t nCount, int32_t nSkill, int32_t nSkillLevel);
// 1.13c D2Game+0xDE20
void __fastcall sub_DE20(D2GameStrc* pGame, D2UnitStrc* pUnit);
// 1.13c D2Game+0xF260
void __fastcall sub_F260(D2UnitStrc* pPicker, D2UnitStrc* pItem);
// 1.13c D2Game+0xF2F0
int32_t __fastcall sub_F2F0(D2UnitStrc* pUnit);
// 1.13c D2Game+0xF180
int32_t __fastcall sub_F180(int32_t nLevel, int32_t nClass, const D2ItemsTxt* pItemsTxt, D2SeedStrc* pSeed, int32_t nFlag);
// 1.13c D2Game+0xF780
int32_t __fastcall sub_F780(D2GameStrc* pGame, D2SeedStrc* pSeed, int32_t nLevel, int32_t nClass, int32_t nFlag, int32_t bAllowType28);
// 1.13c D2Game+0xF8F0
int32_t __fastcall sub_F8F0(int32_t nClass, D2GameStrc* pGame, D2SeedStrc* pSeed, int32_t nLevel, int32_t nFlag);
// 1.13c D2Game+0xFA30
int32_t __fastcall sub_FA30(int32_t nClass, D2GameStrc* pGame, D2SeedStrc* pSeed, int32_t nLevel, int32_t nFlag);
