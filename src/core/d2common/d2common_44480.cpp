// D2Common 0x44480: allocate a 0x20-byte node from the pool in ECX (DrlgRoom.cpp line 0x3B6), clear it, fill it from
// the five stack words (+0x14, +4, +0, +8, +0x18 in that order) and, with a room in ESI, push it on the list at
// room+0x5C (old head into +0xC); with no room +0xC stays 0. Returns the node in EAX; stdcall, ret 0x14.
// The disassembly corrects the decompile, which showed two stack words and the return address at +0.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr32(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

// 1.13c D2Common+0x44480
extern "C" uint32_t D2Common_44480(uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line), uint32_t pool,
	uint32_t room, uint32_t s0, uint32_t s1, uint32_t s2, uint32_t s3, uint32_t s4)
{
	const uint32_t n = alloc(pool, 0x20, 0x3B6);
	for (uint32_t o = 0; o < 0x20; o += 4) wr32(n + o, 0);
	wr32(n + 0x14, s0);
	wr32(n + 0x04, s1);
	wr32(n + 0x00, s2);
	wr32(n + 0x08, s3);
	wr32(n + 0x18, s4);
	if (room) { wr32(n + 0x0C, rd32(room + 0x5C)); wr32(room + 0x5C, n); }
	else wr32(n + 0x0C, 0);
	return n;
}
