// 1.13c D2Common implementation batch: +0x4E2E0.
// Two-level getter: a unit's +0xA8 pointer, when present, exposes the dword at +0x14. No callees.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

// 1.13c D2Common+0x4E2E0 (#10549): unit+0xA8 -> +0x14, else 0.
extern "C"
EMSCRIPTEN_KEEPALIVE uint32_t D2haveLightResMalus(uint32_t pUnit)
{
	if (!pUnit)
		return 0;
	const uint32_t nList = W(P(pUnit), 0xA8);
	if (!nList)
		return 0;
	return W(P(nList), 0x14);
}
