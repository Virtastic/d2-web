// 1.13c D2Common+0x3D920, written from the 1.13c disassembly. Level +0x14 = outdoor block od, +0x1C / +0x20 = origin,
// +0x1C4 / +0x1C8 = the 64-bit seed (step: lo * 0x6AC690C5 + hi), +0x1B4 then +0x478 = the memory pool. The seed always
// steps once; its low two bits are a rotating direction. With head = od +0x68 + idx*4 and e = od + idx*0x14: a null head
// ends it. Unless the kind byte at e +0x1F0 is 4, a zeroed 0x14-byte node holding (e +0x1E8, e +0x1EC) is linked in front
// of head and becomes the list head. head takes (e +0x170, e +0x174). Every following node that has a successor becomes
// origin + 8 * cell + 3 + jitter per axis, the jitter being (seed bit + 2) times the direction table entry (x table
// 0x94020, y table 0x94030), one seed step per axis; the direction then advances. The last node takes (e +0xF8, e +0xFC)
// and gets a fresh zeroed successor holding (e +0x80, e +0x84).
#include <cstdint>
#include "d2common_3d920.h"

namespace {
inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
inline void wr(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }
const int32_t kDx[4] = { 1, 0, -1, 0 };   // 0x94020
const int32_t kDy[4] = { 0, 1, 0, -1 };   // 0x94030

uint32_t step(uint32_t level)
{
	const uint64_t v = uint64_t(rd(level + 0x1c4)) * 0x6ac690c5u + rd(level + 0x1c8);
	wr(level + 0x1c4, uint32_t(v)); wr(level + 0x1c8, uint32_t(v >> 32));
	return uint32_t(v);
}

uint32_t node(uint32_t pool, D2Common3D920Alloc alloc)
{
	const uint32_t n = alloc(pool, 0x14, 0x13);
	for (uint32_t o = 0; o < 0x14; o += 4) wr(n + o, 0);
	return n;
}
}

extern "C" void D2Common_3D920_With(uint32_t level, int32_t idx, D2Common3D920Alloc alloc)   // 1.13c D2Common+0x3D920
{
	const uint32_t pool = rd(rd(level + 0x1b4) + 0x478);
	const uint32_t od = rd(level + 0x14);
	uint32_t v = rd(od + 0x68 + uint32_t(idx) * 4);
	uint32_t c = step(level) & 3;
	if (!v) return;
	const uint32_t e = od + uint32_t(idx) * 0x14;
	if (*reinterpret_cast<const uint8_t*>(uintptr_t(e + 0x1f0)) != 4) {
		const uint32_t n = node(pool, alloc);
		wr(n, rd(e + 0x1e8)); wr(n + 4, rd(e + 0x1ec)); wr(n + 0x10, v);
		wr(od + 0x68 + uint32_t(idx) * 4, n);
	}
	wr(v, rd(e + 0x170)); wr(v + 4, rd(e + 0x174));
	v = rd(v + 0x10);
	if (!v) return;
	while (rd(v + 0x10)) {
		const int32_t jx = int32_t((step(level) & 1) + 2) * kDx[c];
		const int32_t jy = int32_t((step(level) & 1) + 2) * kDy[c];
		c = (c + 1) & 3;
		wr(v, rd(level + 0x1c) + (rd(v) << 3) + uint32_t(jx) + 3);
		wr(v + 4, rd(level + 0x20) + (rd(v + 4) << 3) + uint32_t(jy) + 3);
		v = rd(v + 0x10);
		if (!v) return;
	}
	wr(v, rd(e + 0xf8)); wr(v + 4, rd(e + 0xfc));
	const uint32_t n = node(pool, alloc);
	wr(n, rd(e + 0x80)); wr(n + 4, rd(e + 0x84));
	wr(v + 0x10, n);
}
