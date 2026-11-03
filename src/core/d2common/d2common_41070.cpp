// 1.13c D2Common+0x41070: stdcall(level), ret 4. Fifteen footprint stamps (0x42A70) walk left from the last column.
// Retail reads its two globals directly; the env carries them so a probe can pin them:
//   dt        sgptDataTables (the count at +0xC68 must exceed 0x361)
//   lvlPrest  DAT_6FDF0B3C (the table base; the record for 0x361 is at base+0x5B3B0, its nSizeX at +0x28)
// Retail fatal codes: pOutdoors (level+0x14) null: 0x28A; count <= 0x361 or record null: 0x28D; x < 0: 0x294.
// Stamp call: 0x42A70 with ECX=level, EDX=x, stack (y=0, idx=0x361+i, flag=0, extra=0), fastcall (callee pops 4 words).
// Step = (nSizeX with its sign bits & 7) >> 3; x starts at pOutdoors->nGridWidth (+0x5C) minus one step.
#include <cstdint>
#include <cstring>
#include "d2common_fatal.h"

struct D2Common41070Env {
	uint32_t dt;
	uint32_t lvlPrest;
	void (*stamp)(uint32_t level, int32_t x, int32_t y, int32_t idx, int32_t flag, int32_t extra);
};

namespace {
uint32_t R32(uint32_t a) { uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
}

extern "C" void D2Common_41070(uint32_t level, const D2Common41070Env* e)
{
	const uint32_t outdoors = R32(level + 0x14);
	if (!outdoors) d2common_fatal(0x28a);
	if (static_cast<int32_t>(R32(e->dt + 0xc68)) <= 0x361) d2common_fatal(0x28d);
	const uint32_t record = e->lvlPrest + 0x5b3b0;
	if (!record) d2common_fatal(0x28d);
	const int32_t w = static_cast<int32_t>(R32(record + 0x28));
	const int32_t step = (w + ((w >> 31) & 7)) >> 3;
	int32_t x = static_cast<int32_t>(R32(outdoors + 0x5c)) - step;
	for (int32_t i = 0; i < 15; ++i, x -= step) {
		if (x < 0) d2common_fatal(0x294);
		e->stamp(level, x, 0, 0x361 + i, 0, 0);
	}
}
