// 1.13c D2Common+0x38FF0: stdcall(unit), ret 4.
// A null unit, a table count not above 0x49, a null record (table base + 0x5C64) or a zero unit+0x5C returns 0.
// The 0x38B00 gate runs with EBX = unit+0x5C, EDI = 0x490000 and the record as the stack word. When it passes, the
// 0x38A80 total runs with the same registers and is returned; otherwise the result is 0. The two callees come in
// as function pointers, so a probe can pin them.
#include <cstdint>
#include <cstring>

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" uint32_t D2Common_38FF0(uint32_t unit, uint32_t dt,
                                   uint32_t (*statGate)(uint32_t statlist, uint32_t key, uint32_t rec),
                                   uint32_t (*statTotal)(uint32_t statlist, uint32_t key, uint32_t rec))
{
	if (!unit) return 0;
	if (static_cast<int32_t>(R32(dt + 0xbd4)) <= 0x49) return 0;
	const uint32_t rec = R32(dt + 0xbcc) + 0x5c64u;
	if (!rec) return 0;
	const uint32_t statlist = R32(unit + 0x5c);
	if (!statlist) return 0;
	if (!statGate(statlist, 0x490000u, rec)) return 0;
	return statTotal(statlist, 0x490000u, rec);
}
