// Stage-A implementations for cached D2Common 1.13c routines. Register arguments which the
// decompiler exposed as EAX/ESI are explicit parameters here; retail table offsets are preserved.
#include <cstdint>
#include <emscripten.h>
#include "D2DataTbls.h"
#include "Drlg/D2DrlgDrlg.h"
#include "DataTbls/LevelsTbls.h"
#include "DataTbls/MonsterTbls.h"

namespace {
template<class T> T rd(const void* p, int o) { return *reinterpret_cast<const T*>(static_cast<const uint8_t*>(p)+o); }
}

// 1.13c D2Common+0x1A130: resolve a level's room-coordinate defaults. The environment callbacks
// represent the still-mapped retail callees and are installed by the caller layer.
extern "C" {
using D2C_LevelCoordFn = int32_t (*)(void*, int32_t*, int32_t*);
using D2C_LevelSetupFn = void (*)(void*);
using D2C_LevelDefaultFn = int32_t (*)(int32_t*, int32_t*);
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_1a130(void* drlg, int32_t levelId, int32_t mode,
 int32_t* x, int32_t* y, D2C_LevelCoordFn special, D2C_LevelCoordFn regular,
 D2C_LevelDefaultFn fallback, D2C_LevelSetupFn setup, D2C_LevelCoordFn center)
{
 if (!x || !y) return 0;
 *x = *y = -1;
 if (!drlg || !sgptDataTables || levelId < 0 || levelId >= sgptDataTables->nLevelsTxtRecordCount) return 0;
 auto* levels = reinterpret_cast<const uint8_t*>(sgptDataTables->pLevelsTxt);
 const bool useSpecial = rd<int32_t>(levels + levelId * 0x9c, 0x90) != 0;
 void* level = nullptr;
 for (void* p = reinterpret_cast<void*>(rd<uintptr_t>(drlg, 0x47c)); p;
      p = reinterpret_cast<void*>(rd<uintptr_t>(p, 0x6b * sizeof(uint32_t))))
  if (rd<int32_t>(p, 0x74 * sizeof(uint32_t)) == levelId) { level = p; break; }
 if (!level) return 0; // allocation belongs to the retail DRLG allocator seam
 if (rd<int32_t>(level, 0x10) == 0 && setup) setup(level);
 int32_t ok = 0;
 if (useSpecial) ok = (mode == 0xD && special) ? special(level, x, y) : (regular ? regular(level, x, y) : 0);
 else if (fallback) ok = fallback(x, y);
 if (!ok && !useSpecial && center) ok = center(level, x, y);
 if (!ok) return 0;
 if (*x == -1 || *y == -1) { *x = rd<int32_t>(level, 0x34) + rd<int32_t>(level, 0x3c)/2; *y = rd<int32_t>(level, 0x38) + rd<int32_t>(level, 0x40)/2; }
 return 1;
}

// 1.13c D2Common+0x2FAC0: select the normal or expansion level-monster list entry.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2fac0(int32_t level, int32_t selector, int32_t difficulty,
 const int32_t* baseTable, const int32_t* normalTable, const int32_t* expansionTable)
{
 if (selector == 0) return baseTable ? baseTable[level * 5] : 0;
 if (selector != 1) return 0;
 const int32_t* selected = difficulty < 0x19a ? normalTable : expansionTable;
 return selected ? selected[level * 5] : 0;
}

// 1.13c D2Common+0x2FB10: select the level cap or the monster's difficulty level.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_2fb10(int32_t table, int32_t column, int32_t monster)
{
 if (table == 0) return ((column == 3) ? 0x65 : 0xD5);
 if (table != 1 || !sgptDataTables || monster < 0 || monster >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
 const auto* r = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + monster * 0x1a8;
 return rd<int16_t>(r, column == 0xf ? 0x38 : 0x36);
}

// 1.13c D2Common+0x30B80: unit eligibility predicate, with the two retail lookup calls injected.
using D2C_UnitLookup = int32_t (*)(uint32_t);
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_30b80(int32_t unitType, uint32_t unit, int32_t table,
 D2C_UnitLookup typeOf, D2C_UnitLookup modeOf)
{
 if (table == 0) {
  if (unitType == 10) return 1;
  if (unitType != 0x12 || !unit || !typeOf || !modeOf) return 0;
  return typeOf(unit) != 0 && modeOf(unit) != 0;
 }
 return table == 1 && unitType == 7;
}

// 1.13c D2Common+0x30C70: reads the correct normal/expansion eligibility record, then applies
// the unit's active-state flags. Table-specific fields are kept as explicit offsets.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_30c70(int32_t index, int32_t table, uint32_t unit,
 int32_t monster, const uint32_t* records, D2C_UnitLookup unitFlags)
{
 if (!records) return 0;
 const uint32_t* rec = nullptr;
 if (table == 0) rec = records + index * 5;
 else if (table == 1 && sgptDataTables && monster >= 0 && monster < sgptDataTables->nMonStatsTxtRecordCount) {
  const auto* m = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + monster * 0x1a8;
  if ((rd<uint8_t>(m, 0x0d) & 1u) && (index == 2 || index == 0xf)) return 1;
  rec = records + (monster < 0x19a ? 0x19a : 0) + index * 5;
 } else return 0;
 if (rec[1]) return 1;
 if (!rec[0] || !unit || !unitFlags) return 0;
 const uint32_t f = static_cast<uint32_t>(unitFlags(unit));
 return (f & 1u) && !(f & 0x1000u);
}

// 1.13c D2Common+0x30D50: return the selected monster's special velocity in 8.8 fixed point.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_30d50(int32_t kind, int32_t monster)
{
 if (!sgptDataTables) return 0;
 if (kind == 0) {
  if (monster < 0 || monster >= sgptDataTables->nMonStatsTxtRecordCount) return 0x4000;
  const auto* m = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + monster * 0x1a8;
  return static_cast<int32_t>(rd<uint8_t>(m, 0x40)) << 8;
 }
 if (kind != 1) return 0;
 if (monster < 0 || monster >= sgptDataTables->nMonStatsTxtRecordCount) return 0x3200;
 const auto* m = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + monster * 0x1a8;
 return static_cast<int32_t>(rd<int16_t>(m, 0x32)) << 8;
}

// 1.13c D2Common+0x323E0: resolve the selected stat's resistance value; composition follows retail.
using D2C_StatValue = int32_t (*)(uint32_t, int32_t);
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_323e0(uint32_t unit, int32_t statId,
 int32_t baseId, int32_t itemValue, int32_t combine, D2C_StatValue readStat)
{
 if (!unit || !readStat || !baseId) return 0;
 const int32_t value = readStat(unit, baseId);
 return combine && value ? (itemValue * value) / (itemValue + value) : value;
}
}
