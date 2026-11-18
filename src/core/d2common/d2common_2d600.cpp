// 1.13c D2Common+0x2D600 (sub_2D600): find the room that holds (x, y) in an owner's room list. Register inputs: EAX = owner,
// EDI = x, ESI = y. The owner's first room is the pointer at +0x10, each room's next is at +0x24, and a room's box is
// origin (+0x34, +0x38) and size (+0x3C, +0x40): x0 <= x < x0+w and y0 <= y < y0+h. A walk that runs out of rooms returns
// (retail leaves EAX unset there, so the descriptor masks the result); a null owner or an empty list is retail's fatal exit
// (assert lines 0x2FD / 0x2FE), which this retail-layout twin answers with 0.
// The vendored DRLG_GetRoomExFromLevelAndCoordinates walks the 1.10f level layout (first room at +0x30), not this one, so the
// probe runs this twin on the vector's blobs instead.
#include <cstdint>
#include <cstring>

namespace
{
	uint32_t rd32(uint32_t a, uint32_t off)
	{
		uint32_t v; std::memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a + off)), 4); return v;
	}
	int32_t rdS(uint32_t a, uint32_t off) { return static_cast<int32_t>(rd32(a, off)); }
	int32_t add32(int32_t a, int32_t b) { return static_cast<int32_t>(static_cast<uint32_t>(a) + static_cast<uint32_t>(b)); }
}

extern "C" uint32_t D2Common_2D600_RoomAtXY(uint32_t owner, int32_t x, int32_t y)
{
	if (!owner) return 0;
	for (uint32_t n = rd32(owner, 0x10); n; n = rd32(n, 0x24))
	{
		const int32_t x0 = rdS(n, 0x34), y0 = rdS(n, 0x38);
		if (x0 <= x && y0 <= y && x < add32(x0, rdS(n, 0x3c)) && y < add32(y0, rdS(n, 0x40)))
			return n;
	}
	return 0;
}
