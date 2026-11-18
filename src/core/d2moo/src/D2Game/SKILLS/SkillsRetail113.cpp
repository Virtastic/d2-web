#include <test_seam.h>
#include "SKILLS/SkillsRetail113.h"
#include "ITEMS/ItemsRetail113.h"

#include <cstring>

// Retail-layout (1.13c) skill routines. Addresses are the 32-bit retail ones; the callees trap here (the game has no
// retail-layout caller) and the proofs replace them through D2Seam. Written from the 1.13c disassembly.

uint32_t D2Retail113_9EE50(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_ACBE(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2Retail113_1D30(uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_AFC50(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_9D230(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_75070(uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_75E10(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_A3CA(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2Retail113_A6A0(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_A60A(uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_A5B6(uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_A6E2(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
uint32_t D2Retail113_232B0(uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o = 0) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline int16_t Rd16s(uint32_t a, uint32_t o = 0) { int16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
template <class T> inline uint32_t Addr(T* p) { return (uint32_t)(uintptr_t)p; }
}

// 1.13c D2Game+0xD850: a skill id outside the table or a null record returns the id. 9EE50 (out-dword = the pGame stack slot) gives a
// class; a negative class, a null unit or a non-player unit returns. ACBE(unit+4, 0, level slot) fills the level slot; 1D30(class, slot)
// gives v; AFC50, 9D230 ([[unit+0x14]+0x28], 0xF), 75070 (a 7-dword block) and 75E10, A3CA(unit, 0x8E, 1) follow. An item A6A0 of the
// target (type 6, id -1 without one) gets A60A, A5B6 and A6E2(item, 0x163, v, 0). Last, rec+0xE4 (signed word) in 1..0x2A returns 232B0;
// otherwise the record's upper half over that word.
uint32_t D2Retail113_D850(int32_t skillId, uint32_t unit, uint32_t game, uint32_t target, uint32_t level)
{
	const uint32_t dt = D2_CALL(D2Retail113_DataTables);
	if (skillId < 0 || skillId >= (int32_t)Rd32(dt, 0xBA0)) return (uint32_t)skillId;
	const uint32_t rec = (uint32_t)skillId * 0x23Cu + Rd32(dt, 0xB98);
	if (!rec) return (uint32_t)skillId;
	volatile uint32_t slot0 = game, slot2 = level;
	const uint32_t cls = D2_CALL(D2Retail113_9EE50, Addr(&slot0), target, 0, (uint32_t)skillId, level);
	if ((int32_t)cls < 0 || !unit) return cls;
	if (Rd32(unit) != 1) return Rd32(unit);
	D2_CALL(D2Retail113_ACBE, Rd32(unit, 4), 0, Addr(&slot2));
	const uint32_t v = D2_CALL(D2Retail113_1D30, cls, slot2);
	D2_CALL(D2Retail113_AFC50, unit, game, v, slot0);
	const uint32_t pd = Rd32(unit, 0x14);
	D2_CALL(D2Retail113_9D230, game, unit, pd ? Rd32(pd, 0x28) : 0, 0xF);
	uint32_t blk[7] = {};
	D2_CALL(D2Retail113_75070, Addr(blk), slot0);
	D2_CALL(D2Retail113_75E10, game, Addr(blk), 1);
	D2_CALL(D2Retail113_A3CA, unit, 0x8E, 1);
	const uint32_t type = target ? Rd32(target) : 6u, gid = target ? Rd32(target, 0xC) : 0xFFFFFFFFu;
	const uint32_t item = D2_CALL(D2Retail113_A6A0, Rd32(game, 0x1C), 0, 0, type, gid);
	if (item)
	{
		D2_CALL(D2Retail113_A60A, unit, item, 1);
		D2_CALL(D2Retail113_A5B6, item, 0x8E);
		D2_CALL(D2Retail113_A6E2, item, 0x163, v, 0);
	}
	const int16_t w = Rd16s(rec, 0xE4);
	if (w > 0 && w < 0x2B) return D2_CALL(D2Retail113_232B0, (uint32_t)(int32_t)w, game, unit, 0);
	return (rec & 0xFFFF0000u) | (uint16_t)w;
}
