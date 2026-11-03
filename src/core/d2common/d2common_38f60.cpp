// 1.13c D2Common+0x38F60: stdcall(unit, flag), ret 8.
// flag 0 gates on the table count above 0x16 and the record at +0x1BD8, with key 0x160000. A nonzero flag gates on a
// count above 0x18 and the record at +0x1E60, with key 0x180000. A null unit, a zero unit+0x5C, a failed gate or a
// null record returns 0 with no call. Otherwise the 0x38A80 total is returned: EBX = unit+0x5C, EDI = key, the record
// is the stack word. The data table base (sgptDataTables) comes in as dt and the total as a function pointer, so a
// probe can pin both.
#include <cstdint>
#include <cstring>

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" uint32_t D2Common_38F60(uint32_t unit, uint32_t flag, uint32_t dt,
                                   uint32_t (*statTotal)(uint32_t statlist, uint32_t key, uint32_t rec))
{
	const bool hi = flag != 0;
	const int32_t gate = hi ? 0x18 : 0x16;
	const uint32_t key = hi ? 0x180000u : 0x160000u;
	const uint32_t off = hi ? 0x1e60u : 0x1bd8u;

	if (!unit) return 0;
	const uint32_t statlist = R32(unit + 0x5c);
	if (!statlist) return 0;
	if (static_cast<int32_t>(R32(dt + 0xbd4)) <= gate) return 0;
	const uint32_t rec = R32(dt + 0xbcc) + off;
	if (!rec) return 0;
	return statTotal(statlist, key, rec);
}
