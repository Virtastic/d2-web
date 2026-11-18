#include <test_seam.h>
#include "UNIT/NpcRetail113.h"

#include <cstring>

// Retail-layout (1.13c) hireling/summon link teardown. Written from the 1.13c disassembly. Addresses are the 32-bit retail ones (the
// replay's blobs sit below 4 GiB); every callee (97B50, A478, A44E, E20A0, A4A2, A3A0, A472, 96D70, EFE40, A3CA, BCA20, A3B8, A454,
// 97340, 48950, A4C0) goes through D2_CALL on a stub that traps in the game (it has no retail-layout caller); the FN probe installs
// the hook-log stand-ins.
uint32_t D2Retail113_NpcCall(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                             uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_NpcEdi(uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_NpcSetHooks(uint32_t (*call)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t,
                                              uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                             void (*edi)(uint32_t, uint32_t))
{
	D2Seam<&D2Retail113_NpcCall>::hook = call;
	D2Seam<&D2Retail113_NpcEdi>::hook = edi;
}

namespace
{
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }

uint32_t Call(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t s0 = 0,
              uint32_t s1 = 0, uint32_t s2 = 0)
{
	return D2_CALL(D2Retail113_NpcCall, rva, a, c, d, b, s, s0, s1, s2, 0u, 0u, 0u, 0u, 0u);
}
}

// Unit in EAX, stack = game, rec (ret 8); nothing is returned. With the record's per-game data (A4C0) linked to a list entry whose
// flag bit 0 is clear (only while the data-table word at +0xBF0 is above 7) the entry's key is looked up in the game's node table
// (+0x1320, by key & 0x7f); a type-1 node resets its unit (+0x2C = -1, +0x30 = 1, +0x28 = game) and is handed to 97B50 / A478 /
// A44E / E20A0. Either way the unit is then stripped (A4A2, A3A0, A472, 96D70, EFE40, three A3CA, flag words +0xC4 |= 0x402000E,
// +0xC8 &= 0xFFFBFFFF, BCA20) and the record's pointer is passed on (97340, 48950). Record types 2 and 4..5 read it as a struct
// (+0x10, +0xC); the others go through A3B8 / A454.
// 1.13c D2Game+0xC0B90
void D2Retail113_C0B90(uint32_t unit, uint32_t game, uint32_t rec, uint32_t dpp)
{
	if (!unit || !rec) return;
	const uint32_t pg = Call(0xA4C0, 0, 0, 0, 0, 0, rec);
	if (pg && (int32_t)rd32(rd32(dpp) + 0xBF0) > 7 && rd32(pg + 0x44)) {
		uint32_t ent = rd32(rd32(rd32(pg + 0x44)) + 0x54);
		while (ent && (rd8(ent) & 1)) ent = rd32(ent + 0x14);
		if (ent) {
			const uint32_t key = rd32(ent + 4);
			uint32_t node = rd32(game + 0x1320 + (key & 0x7f) * 4);
			while (node && rd32(node + 0xC) != key) node = rd32(node + 0xE4);
			if (node) {
				if (rd32(node) == 1) {
					const uint32_t nb = rd32(node + 0x14);
					const uint32_t u = nb ? rd32(nb + 0x28) : 0;
					wr32(u + 0x2C, 0xFFFFFFFFu); wr32(u + 0x30, 1); wr32(u + 0x28, game);
				}
				Call(0x97B50, 0, game, rec, 0, 0, rd32(node + 0xC), 0);
				const uint32_t r = Call(0xA478, 0, 0, 0, 0, 0, node, 1, rd32(node + 0xC));
				Call(0xA44E, 0, 0, 0, 0, 0, r);
				Call(0xE20A0, 0, game, node, 0, 0);
			}
		}
	}
	Call(0xA4A2, 0, 0, 0, 0, 0, unit, 1);
	const uint32_t v = Call(0xA3A0, 0, 0, 0, 0, 0, unit, 0);
	Call(0xA472, 0, 0, 0, 0, 0, unit, 6, v);
	Call(0x96D70, unit, 0, 0, 0, 0, game, rec);
	Call(0xEFE40, 0, game, 0, 0, 0, 0);
	Call(0xA3CA, 0, 0, 0, 0, 0, unit, 1, 0);
	Call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x6B, 0);
	Call(0xA3CA, 0, 0, 0, 0, 0, unit, 0x9A, 0);
	wr32(unit + 0xC4, rd32(unit + 0xC4) | 0x402000Eu);
	wr32(unit + 0xC8, rd32(unit + 0xC8) & 0xFFFBFFFFu);
	Call(0xBCA20, 0, 0, 0, unit, 0);
	const int32_t ty = (int32_t)rd32(rec);
	const bool st = ty == 2 || (ty > 3 && ty <= 5);
	const uint32_t p = rd32(rec + 0x2C);
	uint32_t arg1 = 0, arg2 = 0;
	if (st) { arg1 = rd32(p + 0x10); arg2 = rd32(p + 0xC); }
	else {
		if (p) arg1 = Call(0xA3B8, 0, 0, 0, 0, 0, p);
		if (p) arg2 = Call(0xA454, 0, 0, 0, 0, 0, p);
	}
	Call(0x97340, game, 0, 0, 0, 0, rec, arg2, arg1);
	Call(0x48950, rec, game, 0, 0, 0);
}
