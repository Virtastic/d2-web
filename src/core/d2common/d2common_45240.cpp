// D2Common 0x45240: push a 0x18-byte preset node on the list at room+0x4C (EDI = room; stack: payload, file index
// byte; ret 8). Written from the 1.13c disassembly: the decompile hid the EBX/DL inputs of 0x6C5D0.
#include <cstdint>
static inline uint32_t rd32(uint32_t a) { return *reinterpret_cast<const uint32_t*>(uintptr_t(a)); }
static inline void wr32(uint32_t a, uint32_t v) { *reinterpret_cast<uint32_t*>(uintptr_t(a)) = v; }

struct D2Common45240Env {
	uint32_t (*alloc)(uint32_t pool, uint32_t size, uint32_t line);   // 0x9222 (Fog 10045), ECX pool, EDX size
	uint32_t (*levelDef)(uint32_t drlg, uint32_t levelId);            // 0x19950: EAX drlg, EDX level id; returns an array
	uint32_t (*fileEntry)(uint32_t key, uint32_t type);               // 0x6C5D0: EBX key, DL type ('b')
};

// 1.13c D2Common+0x45240
extern "C" void D2Common_45240_With(uint32_t room, uint32_t payload, uint32_t index, const D2Common45240Env* e)
{
	const uint32_t lv = rd32(room + 0x58);
	const uint32_t n = e->alloc(rd32(rd32(lv + 0x1B4) + 0x478), 0x18, 0x322);
	for (uint32_t o = 0; o < 0x18; o += 4) wr32(n + o, 0);
	wr32(n, payload);
	const uint32_t lv2 = rd32(room + 0x58);
	const uint32_t arr = e->levelDef(rd32(lv2 + 0x1B4), rd32(lv2 + 0x1D0));
	const uint32_t key = rd32(arr + (index & 0xFF) * 4);
	wr32(n + 0x10, e->fileEntry(key, 0x62));
	wr32(n + 0x08, 1);
	wr32(n + 0x04, rd32(room + 0x4C));
	wr32(room + 0x4C, n);
}
