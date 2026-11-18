#include <test_seam.h>
#include "MONSTER/MonsterSummonRetail113.h"
#include "D2DataTbls.h"

#include <cstring>

// Retail-layout (1.13c) owned-group spawner, written from the 1.13c decompile. The callees trap here (the game has no
// retail-layout caller for them); the FN probe replaces them through D2Seam.
#define D2R_TRAP() __builtin_trap()
int32_t D2Retail113_21F60(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_MonStatsCount() { return (uint32_t)sgptDataTables->nMonStatsTxtRecordCount; }
int32_t D2Retail113_11D0(uint32_t, int32_t) { D2R_TRAP(); }
uint32_t D2Retail113_EF570(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_EDDB0(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_23060(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_20CF0(uint32_t, uint32_t) { D2R_TRAP(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o = 0) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline void Wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
// the monster data of a unit that is a monster (type 1), or 0
inline uint32_t MonData(uint32_t unit) { return Rd32(unit) == 1 ? Rd32(unit, 0x14) : 0; }
// the AI link of a minion (data +0x28); a null link is written through as in retail
inline uint32_t AiLink(uint32_t unit) { const uint32_t d = MonData(unit); return d ? Rd32(d, 0x28) : 0; }
}

// 1.13c D2Game+0x245A0
void D2Retail113_245A0(int32_t minCount, uint32_t owner, uint32_t game, uint32_t mode, int32_t maxCount)
{
	if (owner != 0)
	{
		const uint32_t d = MonData(owner);
		if (d != 0 && (*((const uint8_t*)(uintptr_t)d + 0x16) & 4)) return;
	}
	const int32_t cls = D2_CALL(D2Retail113_21F60, owner);
	if (cls < 0 || cls >= (int32_t)D2_CALL(D2Retail113_MonStatsCount)) return;
	int32_t count = D2_CALL(D2Retail113_11D0, owner + 0x20, maxCount - minCount + 1) + minCount;
	if (count <= 0) return;
	do
	{
		const uint32_t u = mode != 0 ? D2_CALL(D2Retail113_EF570, 1u, owner, (uint32_t)cls, game, mode, 3u, 0x40u)
		                             : D2_CALL(D2Retail113_EDDB0, 1u, owner, (uint32_t)cls, game, 3u, 0x40u);
		if (u == 0) continue;
		D2_CALL(D2Retail113_23060, u, owner, 0u);
		const uint32_t ownerId = owner ? Rd32(owner, 0xc) : 0xffffffffu;
		if (Rd32(u) == 1)
		{
			const uint32_t link = AiLink(u);
			Wr32(link, 0x2c, ownerId); Wr32(link, 0x30, 1); Wr32(link, 0x28, game);
		}
		D2_CALL(D2Retail113_20CF0, owner, game);
		if (Rd32(u) == 1)
		{
			const uint32_t link = AiLink(u);
			if (owner) { Wr32(link, 0xc, Rd32(owner, 0xc)); Wr32(link, 0x10, Rd32(owner)); }
			else { Wr32(link, 0xc, 0xffffffffu); Wr32(link, 0x10, 1); }
		}
		if (Rd32(u) == 1)
		{
			const uint32_t d = MonData(u);
			if (d) *((uint8_t*)(uintptr_t)d + 0x16) |= 0x10;
		}
	} while (--count != 0);
}
