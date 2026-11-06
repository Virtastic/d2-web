// D2Common 0x43580 (EAX level, plain ret). Written from the 1.13c disassembly: the four grid allocations take the
// pool in ECX and the grid in EBX; the room placer gets EAX = 0 and ECX = level+0x1C; the clipper EBX pool, EDI list.
#include <cstdint>
#include "d2common_43580.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x43580
extern "C" void D2Common_43580_With(uint32_t level, const D2Common43580Env* e)
{
	const uint32_t pool = rd(rd(level + 0x1B4) + 0x478);
	const uint32_t o = rd(level + 0x14);
	wr(o + 0x54, 0);
	wr(o + 0x58, 0);
	const int32_t w = int32_t(rd(level + 0x24)) / 8, h = int32_t(rd(level + 0x28)) / 8;   // cdq/and 7/sar 3 truncates
	wr(o + 0x5C, uint32_t(w));
	wr(o + 0x60, uint32_t(h));
	for (uint32_t g = 4; g <= 0x40; g += 0x14) e->gridInit(pool, o + g, w, h);
	const int32_t id = int32_t(rd(level + 0x1D0));
	uint8_t act = 0;
	for (int i = 1; i <= 5; ++i) if (id < e->actStart[i]) { act = uint8_t(i - 1); break; }
	e->placeRooms(level + 0x1C, pool, o + 0x64, rd(o + 0x264));
	e->clipRooms(pool, o + 0x64);
	if (id == 0x86) act = 1;
	switch (act) {
	case 0: e->act(0x7F3A0, level); break;
	case 1: e->act(0x85E20, level); break;
	case 2: e->act(0x176F0, level); e->act(0x854C0, level); e->act(0x856C0, level); e->act(0x84DA0, level); break;
	case 3: e->act(0x861A0, level); break;
	case 4: e->act(0x41CA0, level); break;
	}
	e->build(level);
}
