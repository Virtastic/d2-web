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

extern "C" {
// 1.13c D2Common+0x2FAC0: select the normal or expansion level-monster list entry.
EMSCRIPTEN_KEEPALIVE int32_t d2_batch_d2common_2fac0(int32_t level, int32_t selector, int32_t difficulty,
 const int32_t* baseTable, const int32_t* normalTable, const int32_t* expansionTable)
{
 if (selector == 0) return baseTable ? baseTable[level * 5] : 0;
 if (selector != 1) return 0;
 const int32_t* selected = difficulty < 0x19a ? normalTable : expansionTable;
 return selected ? selected[level * 5] : 0;
}

// 1.13c D2Common+0x30B80: unit eligibility predicate, with the two retail lookup calls injected.
// Named d2c_30b80_eligible: the FN probe owns the d2_fn_d2common_30b80 symbol (r[] shape).
using D2C_UnitLookup = int32_t (*)(uint32_t);
EMSCRIPTEN_KEEPALIVE int32_t d2c_30b80_eligible(int32_t unitType, uint32_t unit, int32_t table,
 D2C_UnitLookup typeOf, D2C_UnitLookup modeOf)
{
 if (table == 0) {
  if (unitType == 10) return 1;
  if (unitType != 0x12 || !unit || !typeOf || !modeOf) return 0;
  if (typeOf(unit) == 0) return 0;
  // modeOf is retail 0x1250: yields the skills record; its byte +0x12 must be 10
  const auto rec = static_cast<uint32_t>(modeOf(unit));
  return rec != 0 && rd<uint8_t>(reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(rec)), 0x12) == 10;
 }
 return table == 1 && unitType == 7;
}

// 1.13c D2Common+0x30C70: reads the correct normal/expansion eligibility record, then applies
// the unit's active-state flags. The three records tables are retail's contiguous 0x14-byte-row modulator
// tables (player, monster, expansion monster); unitFlags is the word at +0xC of what 0x303A0 answers.
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_30c70_batch(int32_t index, int32_t table, uint32_t unit,
 int32_t monster, const uint32_t* tbl0, const uint32_t* tbl1Lo, const uint32_t* tbl1Hi, D2C_UnitLookup unitFlags)
{
 const uint32_t* rec = nullptr;
 if (table == 0) rec = tbl0 ? tbl0 + index * 5 : nullptr;
 else if (table == 1) {
  // in-range monster flagged (+0x0d & bit0) answers 1 only for index 2 / 0xf; otherwise (or out of range) fall to the table
  if (sgptDataTables && monster >= 0 && monster < sgptDataTables->nMonStatsTxtRecordCount) {
   const auto* m = reinterpret_cast<const uint8_t*>(sgptDataTables->pMonStatsTxt) + monster * 0x1a8;
   if (rd<uint8_t>(m, 0x0d) & 1u) return (index == 2 || index == 0xf) ? 1 : 0;
  }
  const uint32_t* base = monster < 0x19a ? tbl1Lo : tbl1Hi;
  rec = base ? base + index * 5 : nullptr;
 } else return 0;
 if (!rec) return 0;
 if (rec[1]) return 1;
 if (!rec[0] || !unit || !unitFlags) return 0;
 const uint32_t f = static_cast<uint32_t>(unitFlags(unit));
 return (f & 1u) && !(f & 0x1000u);
}

// 1.13c D2Common+0x323E0: resolve the selected stat's resistance value; composition follows retail.
using D2C_StatValue = int32_t (*)(uint32_t, int32_t);
EMSCRIPTEN_KEEPALIVE int32_t d2_fn_d2common_323e0_batch(uint32_t unit, int32_t statId,
 int32_t baseId, int32_t itemValue, int32_t combine, D2C_StatValue readStat)
{
 if (!unit || !readStat || !baseId) return 0;
 const int32_t value = readStat(unit, baseId);
 return combine && value ? (itemValue * value) / (itemValue + value) : value;
}
}
