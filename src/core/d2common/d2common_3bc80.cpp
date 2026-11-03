// 1.13c D2Common+0x3BC80: stdcall(pMemPool, nFlags, ppLevels, pnLevels), ret 0x10.
// Retail reads the portal table count from its global (0xA1424) and the table from its pointer global (0xA1420).
// A count above 0x20 is fatal 0x60A. Each set bit of nFlags, from bit 0 up, takes the table entry at that index into
// a local list. A non-empty list is copied into an allocation from the pool (the pool import takes file tag and line).
// The count and the allocation pointer are written to the two out words; an empty list writes null and zero.
// The count and table come in as arguments and the allocator as a function pointer, so a probe can pin them.
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

namespace {
constexpr uint32_t kPoolFileTag = 0x6FDE4098u;   // the pool tag the caller passes as the file argument
constexpr uint32_t kPoolLine = 0x614u;           // and the line argument
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
void W32(uint32_t a, uint32_t v) { std::memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
}

extern "C" void D2Common_3BC80_PortalLevelArray(uint32_t pMemPool, uint32_t nFlags, uint32_t ppLevels, uint32_t pnLevels,
                                                uint32_t portalCount, uint32_t portalTable,
                                                uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t file, uint32_t line, uint32_t flags))
{
	if (portalCount > 0x20) d2common_fatal(0x60a);

	uint32_t levels[32];
	uint32_t n = 0;
	for (uint32_t i = 0; i < portalCount; ++i)
		if (nFlags & (1u << i)) levels[n++] = R32(portalTable + 4 * i);

	if (n) {
		const uint32_t p = alloc(pMemPool, 4 * n, kPoolFileTag, kPoolLine, 0);
		for (uint32_t i = 0; i < n; ++i) W32(p + 4 * i, levels[i]);
		W32(ppLevels, p);
	} else {
		W32(ppLevels, 0);
	}
	W32(pnLevels, n);
}
