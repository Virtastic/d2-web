// 1.13c D2Common+0x74400: lowqualityitems loader.
// The generic table loader (0x5EF40) and the Fog lookup (ordinal 10010, through thunk 0x92DC) are host callbacks in
// D2Common74400Env, so the same port runs under the FN probe and in the engine.
#include <cstdint>

struct D2Common74400Env
{
	uint32_t (*pfnLoad)(uint32_t archive, uint32_t* pCount);        // 0x5EF40: returns the record block
	uint16_t (*pfnLookup)(uintptr_t pRecord, int32_t* pLocal);      // thunk 0x92DC -> Fog 10010
};

namespace
{
	constexpr uintptr_t kRecordStride = 0x22;
	constexpr uintptr_t kLookupOffset = 0x20;
}

// ECX = archive. Loads the block and count, then stores the lookup word of each record at record+0x20.
// The lookup's dword local starts at 0 and is shared by every record of the call.
extern "C"
void D2Common_74400_Load(uint32_t archive, uint32_t* pCount, uint32_t* pBlock, const D2Common74400Env* pEnv)
{
	*pBlock = pEnv->pfnLoad(archive, pCount);

	int32_t nLocal = 0;
	const uintptr_t pBase = static_cast<uintptr_t>(*pBlock);
	for (uint32_t i = 0; i < *pCount; ++i)
	{
		const uintptr_t pRecord = pBase + static_cast<uintptr_t>(i) * kRecordStride;
		const uint16_t nWord = pEnv->pfnLookup(pRecord, &nLocal);
		*reinterpret_cast<uint16_t*>(pRecord + kLookupOffset) = nWord;
	}
}
