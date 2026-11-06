// D2Common 0x453D0 (stdcall p1, room, idx; ret 0xC). Written from the 1.13c disassembly. 0x452C0 adds 4 to the whole
// ECX, so the slot register is modelled exactly: the first search clears CL over a small counter (upper bytes 0), the
// fallback search loads the idx dword into ECX and replaces only CL.
#include <cstdint>
#include "d2common_453d0.h"

static inline uint32_t rd(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }

// 1.13c D2Common+0x453D0
extern "C" void D2Common_453D0_With(uint32_t p1, uint32_t room, uint32_t idx, const D2Common453d0Env* e)
{
	if (!room || uint8_t(idx) >= 8) { e->fatal(0x350); return; }
	const uint32_t lv = rd(room + 0x58);
	const uint32_t id = rd(lv + 0x1D0);
	const uint32_t links = e->linkTable(rd(lv + 0x1B4), id);
	const uint32_t i = uint8_t(idx);
	const uint32_t target = rd(links + i * 4);
	uint32_t node = rd(rd(lv + 0x1B4) + 0x47C);
	while (node && rd(node + 0x1D0) != target) node = rd(node + 0x1AC);
	if (!node) node = e->newLevel(rd(lv + 0x1B4), target);
	const uint32_t back = e->linkTable(rd(lv + 0x1B4), target);
	const uint32_t v = rd(e->fileTable(rd(lv + 0x1B4), rd(lv + 0x1D0)) + i * 4);
	if (rd(node + 0x10) == 0) e->initLevel(node);
	if (v != 0xFFFFFFFFu) {
		uint32_t cnt = 0;
		for (uint32_t k = 0; k < i; ++k) if (rd(links + k * 4) == target) ++cnt;
		uint32_t seen = 0;
		for (uint32_t s = 0; s < 8; ++s) {
			if (rd(back + s * 4) != id) continue;
			if (cnt == seen) {
				if (e->place(s, rd(node + 0x10), room, p1, idx, v)) return;
				break;
			}
			++seen;
		}
	}
	for (uint32_t s = 0; s < 8; ++s)
		if (rd(back + s * 4) == id && e->place((idx & 0xFFFFFF00u) | s, rd(node + 0x10), room, p1, idx, v)) return;
}
