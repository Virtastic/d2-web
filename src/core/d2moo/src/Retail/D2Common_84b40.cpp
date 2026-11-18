// Monster drop-room lookup, moved from the stand-alone d2common batch 84b40 into the game tree.
#include <cstdint>
#include <emscripten.h>

#include "D2DataTbls.h"
#include "DataTbls/MonsterTbls.h"

// 1.13c D2Common+0x84B40 (#10491) D2GetDropRoom: index the MonStats2 composit table of a monster
// row's wMonStatsEx: component nComponent (0..15) choice nChoice selects a byte in the 0x26 array
// (0xC per component), which indexes the global drop-room table. Every out-of-range index answers 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2GetDropRoom(int32_t nMonsterId, uint32_t nComponent,
	uint32_t nChoice)
{
	if (nComponent > 0xF) return 0;
	if (nMonsterId < 0 || nMonsterId >= sgptDataTables->nMonStatsTxtRecordCount) return 0;
	if (!sgptDataTables->pMonStatsTxt) return 0;

	const int32_t nMonStats2 = sgptDataTables->pMonStatsTxt[nMonsterId].wMonStatsEx;
	if (nMonStats2 < 0) return 0;
	if (nMonStats2 >= sgptDataTables->nMonStats2TxtRecordCount) return 0;
	if (!sgptDataTables->pMonStats2Txt) return 0;

	const D2MonStats2Txt* pRow = &sgptDataTables->pMonStats2Txt[nMonStats2];
	if (nChoice >= pRow->nComponentChoiceCounts[nComponent]) return 0;

	const uint32_t nIndex = ((const uint8_t*)&pRow->unk0x26[nComponent])[nChoice];
	// DAT_6fdeff54/5c in 1.13c are the compcode table the loader fills (sgptDataTables->pCompCodeTxt and its count).
	if ((int32_t)nIndex >= sgptDataTables->nCompCodeTxtRecordCount) return 0;
	if (!sgptDataTables->pCompCodeTxt) return 0;
	return sgptDataTables->pCompCodeTxt[nIndex].dwCode;
}
