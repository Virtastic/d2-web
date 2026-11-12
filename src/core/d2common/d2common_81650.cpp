// 1.13c D2Common+0x81650: EAX = room, stack arg0 = grid {base ptr, row-offset table} (ret 4). For every entry of the room's
// list (room+0x54 holds the +0x14 array, count +0x18, 0x30 bytes each) with flags (+0x14) bit 2 set and bits 0x1c000 clear, ORs 8 into the grid
// cell at base[offsets[entry+0xC] + entry+8] (retail calls 0x36C10, or [ecx], edx, through the pointer at 0x9B244).
#include <cstdint>
// Returns the list count (+0x18), which retail reloads for the loop test and leaves in EAX.
extern "C" uint32_t d2c_81650_port(uint32_t room, uint32_t grid)
{
	auto W = [](uint32_t a, int o) -> uint32_t& { return *reinterpret_cast<uint32_t*>(uintptr_t(a) + o); };
	uint32_t rd = W(room, 0x54);
	for (uint32_t i = 0, o = 0; i < W(rd, 0x18); i++, o += 0x30) {
		uint32_t e = W(rd, 0x14) + o, f = W(e, 0x14);
		if ((f & 4) && !(f & 0x1c000)) {
			uint32_t idx = W(W(grid, 4), W(e, 0xc) * 4) + W(e, 8);
			W(W(grid, 0), idx * 4) |= 8;
		}
	}
	return W(rd, 0x18);
}
