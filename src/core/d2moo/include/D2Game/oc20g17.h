#pragma once
// 1.13c D2Game functions moved from their stand-alone proof copies into the game tree (batch oc20-g17).
// Each is marked `// 1.13c D2Game+0x<RVA>` where it is defined; the sources sit beside the D2MOO file of their area.

#include <Units/Units.h>
#include <D2Dungeon.h>
#include <Drlg/D2DrlgDrlg.h>
#include <D2States.h>
#include <D2StatList.h>
#include <D2DataTbls.h>
#include <D2Monsters.h>
#include <D2Skills.h>
#include <D2QuestRecord.h>
#include <Units/Monster.h>
#include "UNIT/SUnit.h"
#include "UNIT/SUnitDmg.h"
#include "MONSTER/MonsterUnique.h"
#include "ITEMS/Items.h"

// 1.13c D2Game+0xDAB00
void __fastcall sub_DAB00(D2UnitStrc* pRoomUnit, D2UnitStrc* pRefUnit, int32_t nClassId);

// 1.13c D2Game+0xDB5E0
D2UnitStrc* __fastcall sub_DB5E0(D2UnitStrc* pUnit, D2GameStrc* pGame, D2UnitStrc* pTarget);

// 1.13c D2Game+0xDBA40
void __fastcall sub_DBA40(D2DamageStrc* pDamage, D2UnitStrc* pDefender, D2GameStrc* pGame, D2UnitStrc* pAttacker);

// 1.13c D2Game+0xE34D0
void __fastcall sub_E34D0(D2BitBufferStrc* pQuestRecord, D2UnitStrc* pUnit, D2GameStrc* pGame);

// 1.13c D2Game+0xE390
uint32_t __fastcall sub_E390(D2UnitStrc* pUnit);

// 1.13c D2Game+0x1E900
void __fastcall sub_1E900(D2GameStrc* pGame, D2UnitStrc* pUnit, struct D2AiTickParamStrc* pAiTickParam);
