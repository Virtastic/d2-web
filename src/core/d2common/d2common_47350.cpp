// D2Common 0x47350 (EDX, stack s0..s8; ret 0x24). Written from the 1.13c disassembly: s1 is the record (+4 mode,
// +8 x, +0xC y), s3 the stat id (low word, signed), EDX and s0 go to 0x47020 in ECX and EAX with s6/s7 on the stack.
#include <cstdint>
#include "d2common_47350.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }

// 1.13c D2Common+0x47350
extern "C" int32_t D2Common_47350_With(uint32_t edx, uint32_t s0, uint32_t rec, uint32_t stat, uint32_t s6, uint32_t s7,
	const D2Common47350Env13* e)
{
	if (!rec) return 0;
	const int32_t id = int16_t(stat);
	if (id < 0 || id >= int32_t(rd(e->dataTables + 0xBD4)) || uint32_t(id) * 0x144u + rd(e->dataTables + 0xBCC) == 0) return 0;
	int32_t a = int32_t(rd(rec + 4));
	int32_t x = int32_t(rd(rec + 8) + 0x100);
	int32_t y = int32_t(rd(rec + 0xC) + 0x100);
	a = a <= 0 ? 0 : (a >= 3 ? 3 : a);
	x = x <= 0 ? 0 : (x >= 0x3FF ? 0x3FF : x);
	y = y <= 0 ? 0 : (y >= 0x3FF ? 0x3FF : y);
	const uint32_t list = e->statList(s0, edx, s6, s7);
	if (!list) return 0;
	e->addStat(list, uint32_t(id), uint32_t(a) + (uint32_t(y) * 0x400u + uint32_t(x)) * 4u, 0);
	return y;
}
