// D2Common 0x4A5C0 (EAX room flags, ECX level, seven stack words, ret 0x1C; returns the room). Written from the 1.13c
// disassembly: 0x44BD0 takes the level in EBX, 0x83740 the new room in ESI. A level id outside the table reads row 0
// at address 0 in retail (no guard); the port keeps that behaviour.
#include <cstdint>
#include "d2common_4a5c0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x4A5C0
extern "C" uint32_t D2Common_4A5C0_With(uint32_t flags, uint32_t level, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
	uint32_t outFlags, uint32_t outFlagsEx, uint32_t dt1Mask, const D2Common4a5c0Env* e)
{
	const uint32_t room = e->allocRoom(level, 1);
	wr(room + 0x34, x); wr(room + 0x38, y); wr(room + 0x40, h); wr(room + 0x3C, w);
	wr(room + 0x24, rd(level + 0x10));        // push on the level's room list
	wr(level + 0x10, room);
	wr(level + 0x08, rd(level + 0x08) + 1);
	wr(room + 0x28, rd(room + 0x28) | flags | 0x80000);
	const uint32_t outd = rd(room + 0x20);
	wr(room + 0x50, dt1Mask);
	wr(outd + 0x54, outFlags);
	wr(outd + 0x58, outFlagsEx);
	const int32_t id = int32_t(rd(level + 0x1D0));
	const uint32_t def = (id >= 0 && id < int32_t(rd(e->dataTables + 0xC5C))) ? e->levelDefs + uint32_t(id) * 0x9C : 0;
	wr(outd + 0x64, rd(def + 0x38));
	wr(outd + 0x68, rd(def + 0x3C));
	wr(outd + 0x6C, e->pickSubThemes(room, rd(def + 0x38), rd(def + 0x3C)));
	return room;
}
