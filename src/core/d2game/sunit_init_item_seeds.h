// 1.13c D2Game+0xE00C0 SUNIT_InitItemSeeds: the item branch of SUNIT_AllocUnitData, outlined by the 1.13c
// compiler. Retail ABI: EAX = pGameSeed, EDI = pUnit; EAX at return = &pItemData->pSeed.
#pragma once

struct D2UnitStrc;
struct D2SeedStrc;

D2SeedStrc* SUNIT_InitItemSeeds(D2UnitStrc* pUnit, D2SeedStrc* pGameSeed);
