// 1.13c D2Common+0x5D9B0: index list of the states whose wState is positive.
// The retail body allocates one short per states.txt record (Fog pool, file/line site in DataTbls.cpp),
// writes its address through *output and the count through *count, then lists the record indices
#include <cstddef>
#include <cstdint>

#include <Fog.h>
#include "D2DataTbls.h"

static_assert(sizeof(D2StatesTxt) == 0x3C, "states.txt stride");
static_assert(offsetof(D2StatesTxt, wState) == 0x00, "states.txt wState");

// 1.13c D2Common+0x5D9B0
void D2Common_5D9B0_CollectPositiveStates(int16_t** output, int16_t* count)
{
	const int32_t nRecords = sgptDataTables->nStatesTxtRecordCount;
	int16_t* list = static_cast<int16_t*>(D2_CALLOC(uint32_t(nRecords) * 2u));
	*output = list;
	*count = 0;
	for (int32_t i = 0; i < nRecords; ++i)
	{
		if (static_cast<int16_t>(sgptDataTables->pStatesTxt[i].wState) > 0)
		{
			list[*count] = static_cast<int16_t>(i);
			*count = static_cast<int16_t>(*count + 1);
		}
	}
}
