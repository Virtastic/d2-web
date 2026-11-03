// 1.13c D2Common+0x5DA30: index list of the states whose flag bit `bit` is set.
// Same shape as 0x5D9B0: the list is allocated from the Fog pool (file/line site in DataTbls.cpp,
// zero flag) and the count is written through the second output. The bit is tested in the record's
// flag bytes at +0x10 (byte bit >> 3, mask 1 << (bit & 7)). Prior art: the 1.13c decompile cached under
#include <cstddef>
#include <cstdint>

#include <Fog.h>
#include "D2DataTbls.h"

static_assert(sizeof(D2StatesTxt) == 0x3C, "states.txt stride");
static_assert(offsetof(D2StatesTxt, nStateFlags) == 0x10, "states.txt flag bytes");

// 1.13c D2Common+0x5DA30 (bit in EAX, output on the stack, count in ESI)
void D2Common_5DA30_CollectStatesWithBit(int32_t bit, int16_t** output, int16_t* count)
{
	const int32_t nRecords = sgptDataTables->nStatesTxtRecordCount;
	int16_t* list = static_cast<int16_t*>(D2_CALLOC(uint32_t(nRecords) * 2u));
	*output = list;
	*count = 0;
	const uint8_t nMask = static_cast<uint8_t>(1u << (bit & 7));
	for (int32_t i = 0; i < nRecords; ++i)
	{
		if (sgptDataTables->pStatesTxt[i].nStateFlags[bit >> 3] & nMask)
		{
			list[*count] = static_cast<int16_t>(i);
			*count = static_cast<int16_t>(*count + 1);
		}
	}
}
