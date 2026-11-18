#pragma once
// Retail-layout (1.13c) item routines of D2Game.dll: the item roll / table pick functions whose arguments are the 32-bit
// retail addresses of the item, class record and D2Common tables (the game's own structs are 64-bit and the retail layouts
// differ). The D2Common ordinals they call are declared here by ordinal; the game has no retail-layout caller for them,
// so the definitions trap -- the proofs install stand-ins through D2Seam.
#include <cstdint>

// D2Common ordinals / D2Game helpers (retail addresses as uint32_t)
uint32_t D2Retail113_Ord10695_ItemClassRecord(uint32_t classId);
uint32_t D2Retail113_Ord10718_ItemFormat(uint32_t item);
int32_t D2Retail113_Ord10086_ItemLevelCap(uint32_t item);
uint32_t D2Retail113_Ord10411_ItemSeed(uint32_t item);
void D2Retail113_Ord10536_SetItemRow(uint32_t item, uint32_t row);
void D2Retail113_Ord10977_ItemFlag(uint32_t item, uint32_t a, uint32_t b);
void D2Retail113_Ord10996_ItemEvent(uint32_t a, uint32_t b, uint32_t item, uint32_t c, uint32_t d, uint32_t e);
uint32_t D2Retail113_Ord10926_Table();
uint32_t D2Retail113_Roll1180(uint32_t seed, uint32_t nMax);
uint32_t D2Retail113_Roll11D0(uint32_t seed, uint32_t nMax);
uint32_t D2Retail113_Ord10865_ItemFlag(uint32_t item);
void D2Retail113_Ord10887_SetStat(uint32_t item, uint32_t stat, uint32_t value, uint32_t layer);
uint32_t D2Retail113_Ord10744_IsType(uint32_t item, uint32_t type);
int32_t D2Retail113_Ord10587_GetStat(uint32_t item, uint32_t stat, uint32_t layer);
uint32_t D2Retail113_Ord10711_ItemFlag(uint32_t item);
int32_t D2Retail113_Ord10910_GetBaseStat(uint32_t item, uint32_t stat, uint32_t layer);
int32_t D2Retail113_Sub13230_Scale(int32_t a, int32_t floorValue, int32_t pct);
void D2Retail113_Sub13DE0_Finish(uint32_t item, uint32_t arg);
uint32_t D2Retail113_Ord10295_SlotTable();
uint32_t D2Retail113_Ord11007_SlotType(uint32_t slot);
uint32_t D2Retail113_Ord11094_TypeAccepts(uint32_t item, uint32_t type);
uint32_t D2Retail113_Ord10220_PickTable();
uint32_t D2Retail113_Ord10110_ElementAccepts(uint32_t unit, uint32_t element);
uint32_t D2Retail113_DataTables();
uint32_t D2Retail113_Sub13A30_NoFormat();
// 144D0 callees (Ord = D2Common thunk, others D2Game)
uint32_t D2Retail113_SgptRowTable_B39C();
uint32_t D2Retail113_UnitSeed_A5EC(uint32_t unit);
uint32_t D2Retail113_UnitWord_B37E(uint32_t unit);
uint32_t D2Retail113_UnitByte_B390(uint32_t unit);
uint32_t D2Retail113_UnitLevel_A8F8(uint32_t unit);
uint32_t D2Retail113_MonRecord_A53E(uint32_t classId);
uint32_t D2Retail113_UnitKind_A898(uint32_t unit);
uint32_t D2Retail113_RowAllowed_B3C0(uint32_t unit, uint32_t row);
uint32_t D2Retail113_UnitDiff_B3D8(uint32_t unit);
uint32_t D2Retail113_RowBlocked_131E0(uint32_t unit, uint32_t id);
void D2Retail113_Announce_A9D0(uint32_t a, uint32_t b, uint32_t unit, uint32_t row);

// 1.13c D2Game+0x13C20 (item, client; weighted drop-table roll), +0x13E60, +0x140B0, +0x141E0, +0x14210, +0x14370, +0x144D0
uint32_t D2Retail113_13C20(uint32_t item, uint32_t client);
int32_t D2Retail113_13E60(uint32_t item, uint32_t arg);
uint32_t D2Retail113_140B0(uint32_t item, uint32_t arg);
uint32_t D2Retail113_141E0(uint32_t item, uint32_t arg);
uint32_t D2Retail113_14210(uint32_t unit, uint32_t flag);
uint32_t D2Retail113_14370(uint32_t unit, uint32_t flag);
uint32_t D2Retail113_144D0(uint32_t unit, uint32_t needFlag, uint32_t bypass, uint32_t announce, uint32_t second, int32_t forced, uint32_t idFilter);
