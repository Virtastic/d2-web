// D2Common 0x475E0 (EAX gate, stack s0..s7; ret 0x20). Written from the 1.13c disassembly: s2 picks add (0x3A280) or
// set (0x3A330), s3 is the stat id (low word, signed), s5 the amount (shifted by stat row byte +0x18), s4 rides as the
// fourth stack word of the add/set call. Stat 0x3A also touches stat 0x146 (id<<16 = 0x1460000 in EAX/EDI).
#include <cstdint>
#include "d2common_475e0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x475E0
extern "C" int32_t D2Common_475E0_With(uint32_t eax, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4,
	uint32_t s5, uint32_t s6, uint32_t s7, const D2Common475e0Env* e)
{
	if (!eax || !s5) return 0;
	const int16_t sid = int16_t(s3);
	const int32_t id = sid;
	if (id < 0 || id >= int32_t(rd(e->dataTables + 0xBD4))) return 0;
	const uint32_t row = uint32_t(id) * 0x144u + rd(e->dataTables + 0xBCC);
	if (!row) return 0;
	const uint32_t list = e->statList(s1, s0, s6, s7);
	if (!list) return 0;
	const uint32_t v = s5 << (*reinterpret_cast<const uint8_t*>(uintptr_t(row + 0x18)) & 31);
	if (!s2) {
		e->setStat(list, uint32_t(id), v, s4);
		if (sid == 0x3A) e->setStat(list, 0x146, 1, 0);
		return int32_t(s5);
	}
	e->addStat(list, uint32_t(id), v, s4);
	if (sid != 0x3A || e->getStat(list, 0x146, 0) != 0) return int32_t(s5);
	const uint32_t arr = list + 0x24;
	const int32_t i = e->findSlot(arr, 0x1460000);
	uint32_t p = i < 0 ? 0 : rd(arr) + uint32_t(i) * 8u;
	if (!p) p = e->newSlot(0x1460000, arr, rd(list));
	const uint32_t delta = 1u - rd(p + 4);
	if (!delta) return int32_t(s5);
	wr(p + 4, 1);
	e->changed(list, 0x1460000, delta, 0);
	if (int32_t(rd(list + 0x10)) < 0 && rd(list + 8) == 0) e->flush(0x1460000, list);
	return int32_t(s5);
}
