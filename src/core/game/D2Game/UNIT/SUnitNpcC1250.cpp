#include <test_seam.h>
#include "UNIT/NpcRetail113.h"
#include "SUnitNpcCommon.h"

using namespace npc113;

// Retail-layout (1.13c) create-a-mercenary: EBX = game, stack = player, name word, seed, version (short; -1 picks the record by act
// and difficulty), class id (0: the hireling record's), dead flag (ret 0x18). Returns the new unit or 0. ABF8 gives the hireling act
// for the name; without the expansion flag (game+0x70) the player's act (player+0x18, or the client's +0x1ac when the player has
// one) must equal it. A3BE / AC52 fetch the hireling record (never null in the captured corpus: the original asserts and exits).
// The name is clamped into the record's [first, last] range; the seller NPC's merc slot (8F5C0 -> +0x10, 16-byte entries) is marked
// hired when not already present. The class must be a monstats row whose byte +0xf shares a bit with gdwBitMasks[1]; then A6B8
// allocates the unit, the fields are set inline, the unit id comes from game+0x94 (wraps past -1 to 1), and the unit is registered
// (B0320, E0F40), flagged (A688/13B0 path) and handed to 48FC0 with a stack block {word name, residue, dword seed}. dtCell =
// &sgptDataTables, bm = gdwBitMasks. Every callee goes through D2_CALL on the NpcCall stub.
// 1.13c D2Game+0xC1250
uint32_t D2Retail113_C1250(uint32_t game, uint32_t player, uint32_t name, uint32_t seed, uint32_t ver, uint32_t cls, uint32_t dead,
                           uint32_t dtCell, uint32_t bm)
{
	const uint32_t act = Call(0xABF8, 0, rd32(game + 0x70), 0, 0, 0, name);   // EAX = name too (unlogged)
	uint32_t pact = rd8(player + 0x18);
	if (rd32(player) == 0 && rd32(Call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c) != 0) {
		const uint32_t client = rd32(Call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
		if (!client) return 0;   // the original asserts and exits
		pact = rd8(client + 0x1ac);
	}
	const uint32_t exp = rd32(game + 0x70);
	if (!exp && ((int32_t)act < 0 || pact != act)) return 0;
	uint32_t rec;
	if ((ver & 0xFFFF) != 0xFFFF) rec = Call(0xA3BE, 0, 0, 0, 0, 0, exp, ver & 0xFFFF, 1);
	else rec = Call(0xAC52, 0, 0, 0, 0, 0, exp, act, rd8(game + 0x6d), 0);
	if (!rec) return 0;   // the original asserts and exits
	const uint32_t first = rd16(rec + 0x114);
	if (!cls) cls = rd32(rec + 8);
	uint32_t nm = name & 0xFFFF;
	if (nm < first || nm > rd16(rec + 0x116)) nm = first;
	uint32_t slot = 0;
	const uint32_t npc = Call(0x8F5C0, (uint32_t)(uintptr_t)&slot, game, 0, 0, rd32(rec + 0x14));
	const uint32_t md = rd32(npc + 0x10);
	if (md) {
		const uint32_t m = md + ((nm - first) << 4);
		if (!rd32(m + 0xc)) wr32(m + 8, 1);
	}
	const uint32_t mode = dead ? 12 : 1;
	if ((int32_t)cls < 0) return 0;
	const uint32_t dt = rd32(dtCell);
	if ((int32_t)cls >= (int32_t)rd32(dt + 0xa80)) return 0;
	const uint32_t row = cls * 0x1a8 + rd32(dt + 0xa78);
	if (!row || !(rd8(bm + 4) & rd8(row + 0xf))) return 0;
	const uint32_t u = Call(0xA6B8, 0, 0, 0, 0, 0, rd32(game + 0x1c), 1);
	if (!u) return 0;
	wr32(u + 4, cls); wr32(u, 1); wr32(u + 8, rd32(game + 0x1c)); wr32(u + 0x80, game);
	uint32_t f = rd32(u + 0xc8);
	f = rd32(game + 0x70) ? (f | 0x2000000u) : (f & 0xFDFFFFFFu);
	wr32(u + 0xc8, f | 0x4000000u);
	const uint32_t b = Call(0xA56E, 0, 0, 0, 0, 0, Call(0xA556, 0, 0, 0, 0, 0, 0)) & 0xFF;
	wr8(u + 0x18, b);
	wr32(u + 0x1c, rd32(game + b * 4 + 0xbc));
	wr32(u + 0x74, 0);
	Call(0xE0110, 0, 0, 0, 0, game + 0xd0);
	Edi(0xE0110, u);
	wr32(u + 0xc4, rd32(u + 0xc4) | 0x10);
	wr32(u + 0xd0, 0xb);
	uint32_t id = rd32(game + 0x94) + 1;
	if (id == 0xFFFFFFFFu) id = 1;
	wr32(game + 0x94, id);
	wr32(u + 0x10, mode);
	Call(0xB0320, 0, game, 0, 0, 0, u, id);
	Call(0xE0F40, 0, 0, u, 0, 0, 0, game, 1);
	if (Call(0xA688, 0, 0, 0, 0, 0, u) && !Call(0x13B0, cls, 0, 0x13)) {
		Call(0xA6AC, 0, 0, 0, 0, 0, u, 1);
		Call(0xA6D6, 0, 0, 0, 0, 0, u, 5);
		Call(0xA682, 0, 0, 0, 0, 0, rd32(u + 0x2c), 0x8000);
	}
	uint32_t blk[2] = { nm, seed };
	Call(0x48FC0, ver, player, 0, 0, 0, game, player, u, ver, (uint32_t)(uintptr_t)blk, dead);
	return u;
}
