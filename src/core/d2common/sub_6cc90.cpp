// 1.13c D2Common+0x6CC90 (#10215)
#include <cstdint>
#include "D2DataTbls.h"
#include "DataTbls/LevelsTbls.h"
// Rain flag of levels.txt row nLevelId (row 0 excluded). EAX bits 8..31 keep the imul leftover, as retail.
uint32_t D2Common_6CC90_LevelRain(uint32_t nLevelId)
{
    if ((int32_t)nLevelId > 0 && (int32_t)nLevelId < sgptDataTables->nLevelsTxtRecordCount)
        return ((nLevelId * 0x220u >> 8) << 8) | sgptDataTables->pLevelsTxt[nLevelId].nRain;
    return nLevelId & 0xFFFFFF00u;
}
