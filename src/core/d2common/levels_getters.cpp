// D2Common +0x6CC60 / +0x6CC30: single-field readers over the levels tables
//
#include <cstddef>
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/LevelsTbls.h"
static_assert(sizeof(D2LevelDefBin) == 0x9C, "levelsdef.bin record");
static_assert(offsetof(D2LevelDefBin, dwLevelType) == 0x34, "+0x34");
static_assert(offsetof(D2LevelDefBin, dwSaveMonsters) == 0x94, "+0x94");
static_assert(offsetof(D2LevelDefBin, dwLOSDraw) == 0x98, "+0x98");
static_assert(sizeof(D2LevelsTxt) == 0x220, "levels.txt record");
static_assert(offsetof(D2LevelsTxt, nMud) == 6, "+6");
static_assert(offsetof(D2LevelsTxt, nNoPer) == 7, "+7");
// 0x2D810 / 0x2D850 / 0x2D970 are DRLG_GetLOSDrawFromLevelId / DRLG_GetSaveMonstersFromLevelId / DRLG_GetLevelTypeFromLevelId (DrlgDrlg.cpp).
// levels.txt byte at +7 / +6 for id in (0, count); EAX bits 8..31 keep the imul leftover, else id & 0xFFFFFF00 (as sub_6CC90).
static uint32_t LevelsByte(uint32_t id, bool noPer)
{
    if ((int32_t)id > 0 && (int32_t)id < sgptDataTables->nLevelsTxtRecordCount) {
        const D2LevelsTxt& l = sgptDataTables->pLevelsTxt[id];
        return ((id * 0x220u >> 8) << 8) | (noPer ? l.nNoPer : l.nMud);
    }
    return id & 0xFFFFFF00u;
}
// 1.13c D2Common+0x6CC60
uint32_t D2Common_6CC60_LevelNoPer(uint32_t id) { return LevelsByte(id, true); }
// 1.13c D2Common+0x6CC30
uint32_t D2Common_6CC30_LevelMud(uint32_t id) { return LevelsByte(id, false); }
