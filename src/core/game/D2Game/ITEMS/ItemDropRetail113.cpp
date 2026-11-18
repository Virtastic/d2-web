#include <test_seam.h>
#include "ITEMS/ItemDropRetail113.h"
#include "ITEMS/ItemsRetail113.h"

#include <cstring>

// Retail-layout (1.13c) item quality setters and item-drop builders. Addresses are the 32-bit retail ones; the callees are
// declared in the header and trap here (the game has no retail-layout caller for them). Written from the 1.13c decompile;
// one body per marked function.

#define D2R_TRAP() __builtin_trap()
void D2Retail113_E210() { D2R_TRAP(); }
void D2Retail113_A5B0(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_A628(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_EE90(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_B090(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_AFB2(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_B378(uint32_t) { D2R_TRAP(); }
void D2Retail113_14F20(uint32_t) { D2R_TRAP(); }
void D2Retail113_15240(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_103C0(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_10600(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_10550(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_Ord10757_IsSocketable(uint32_t) { D2R_TRAP(); }
void D2Retail113_E1110(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10820_FreeInventory(uint32_t) { D2R_TRAP(); }
void D2Retail113_8E880(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_Ord10141_UnitCoords(uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10331_UnitRoom(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_E930(uint32_t, uint32_t, int32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_E5D0(uint32_t, int32_t) { D2R_TRAP(); }
int32_t D2Retail113_11490(uint32_t, uint32_t, int32_t) { D2R_TRAP(); }
uint32_t D2Retail113_Ord10630_RoomLevel(uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_A808(uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_ADA8(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_F780(uint32_t, uint32_t, int32_t, uint32_t, uint32_t, int32_t) { D2R_TRAP(); }
uint32_t D2Retail113_1280(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_AE98(uint32_t, uint32_t, uint32_t, int32_t, int32_t, int32_t, int32_t) { D2R_TRAP(); }
int32_t D2Retail113_F8F0(uint32_t, uint32_t, uint32_t, int32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_B1B6(int32_t) { D2R_TRAP(); }
int32_t D2Retail113_FA30(uint32_t, uint32_t, uint32_t, int32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_FFC0(int32_t, uint32_t, int32_t, int32_t, int32_t) { D2R_TRAP(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o = 0) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
inline uint16_t Rd16(uint32_t a, uint32_t o = 0) { uint16_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 2); return v; }
inline uint8_t Rd8(uint32_t a, uint32_t o = 0) { return *((const uint8_t*)(uintptr_t)a + o); }
inline void Wr32(uint32_t a, uint32_t o, uint32_t v) { std::memcpy((uint8_t*)(uintptr_t)a + o, &v, 4); }
inline uint32_t Addr(const void* p) { return (uint32_t)(uintptr_t)p; }

// the 0x84-byte drop request block, kept as retail bytes
struct DropBlock
{
	uint8_t b[0x84];
	DropBlock() { std::memset(b, 0, sizeof b); }
	uint32_t Ptr() const { return Addr(b); }
	void W32(uint32_t o, uint32_t v) { std::memcpy(b + o, &v, 4); }
	void W16(uint32_t o, uint16_t v) { std::memcpy(b + o, &v, 2); }
};

// the state preamble every quality setter shares: up to the state write at drop+0x30
void QualityPre(uint32_t p, uint32_t q, uint32_t drop, uint32_t state)
{
	D2_CALL(D2Retail113_E210);
	D2_CALL(D2Retail113_A5B0, p, q);
	const uint32_t x = D2_CALL(D2Retail113_UnitSeed_A5EC, p);
	D2_CALL(D2Retail113_A628, x, q);
	D2_CALL(D2Retail113_EE90, p, drop);
	D2_CALL(D2Retail113_B090, p, state);
	Wr32(drop, 0x30, state);
}

// the item level the room gives: the level's item level with the game's difficulty (+0x6D) and seed word (+0x70), one less when above 1
int32_t RoomItemLevel(uint32_t game, uint32_t room)
{
	int32_t ilvl = D2_CALL(D2Retail113_ADA8, D2_CALL(D2Retail113_Ord10630_RoomLevel, room), Rd8(game, 0x6D), Rd32(game, 0x70));
	if (ilvl > 1) --ilvl;
	return ilvl;
}

// the drop point nudged by (+2, +3) when 0x1280 finds a room there
void NudgedPoint(uint32_t room, uint32_t org, uint32_t in)
{
	const uint32_t x = Rd32(org, 0), y = Rd32(org, 4);
	if (D2_CALL(D2Retail113_1280, room, x + 2, y + 3)) { Wr32(in, 0, x + 2); Wr32(in, 4, y + 3); }
	else { Wr32(in, 0, x); Wr32(in, 4, y); }
}
}

// 1.13c D2Game+0x101E0 (EAX = pP, EBX = pQ, ESI = drop): superior-quality item roll
void D2Retail113_101E0(uint32_t pP, uint32_t pQ, uint32_t drop)
{
	const uint32_t p = Rd32(pP);
	QualityPre(p, Rd32(pQ), drop, 3);
	const uint32_t y = D2_CALL(D2Retail113_UnitSeed_A5EC, p);
	Wr32(pQ, 0, D2_CALL(D2Retail113_AFB2, y));
	D2_CALL(D2Retail113_UnitWord_B37E, p);
	D2_CALL(D2Retail113_140B0, p, drop);
}

// 1.13c D2Game+0x104F0 (EAX = pP, ESI = pQ, EBX = drop): magic-quality item roll
void D2Retail113_104F0(uint32_t pP, uint32_t pQ, uint32_t drop)
{
	const uint32_t p = Rd32(pP);
	QualityPre(p, Rd32(pQ), drop, 4);
	const uint32_t y = D2_CALL(D2Retail113_UnitSeed_A5EC, p);
	Wr32(pQ, 0, D2_CALL(D2Retail113_AFB2, y));
	D2_CALL(D2Retail113_103C0, pP, drop);
}

// 1.13c D2Game+0x10730 (ECX = pP, EAX = pQ, EBX = drop): rare-quality item roll
void D2Retail113_10730(uint32_t pP, uint32_t pQ, uint32_t drop)
{
	const uint32_t p = Rd32(pP);
	QualityPre(p, Rd32(pQ), drop, 6);
	const uint32_t y = D2_CALL(D2Retail113_UnitSeed_A5EC, p);
	Wr32(pQ, 0, D2_CALL(D2Retail113_AFB2, y));
	if (D2_CALL(D2Retail113_B378, p) == 0) return;
	if ((uint16_t)D2_CALL(D2Retail113_UnitWord_B37E, p) < 1) D2_CALL(D2Retail113_14F20, drop);
	else D2_CALL(D2Retail113_15240, drop);
}

// 1.13c D2Game+0x109B0 (ECX = pP, EAX = Q, ESI = drop, stack = nArg): normal-quality item roll
void D2Retail113_109B0(uint32_t pP, uint32_t nQ, uint32_t drop, uint32_t nArg)
{
	const uint32_t p = Rd32(pP);
	QualityPre(p, nQ, drop, 2);
	if ((uint16_t)D2_CALL(D2Retail113_UnitWord_B37E, p) == 0) D2_CALL(D2Retail113_10600, drop, p, nArg);
	else D2_CALL(D2Retail113_10550, p, drop, nArg);
}

// 1.13c D2Game+0x10700 (EDI = game, ESI = item): a socketable item has its socketed contents removed and its inventory (+0x60) freed; then the timers
void D2Retail113_10700(uint32_t game, uint32_t item)
{
	if (D2_CALL(D2Retail113_Ord10757_IsSocketable, item))
	{
		D2_CALL(D2Retail113_E1110, game, item);
		D2_CALL(D2Retail113_Ord10820_FreeInventory, Rd32(item, 0x60));
	}
	D2_CALL(D2Retail113_8E880, game, item);
}

// 1.13c D2Game+0x11880 (EBX = unit): item drop at a unit; a negative index or no free spot answers 0
int32_t D2Retail113_11880(uint32_t unit, uint32_t game, int32_t nIndex, uint32_t a3, uint32_t a4, uint32_t nFlags)
{
	if (nIndex < 0) return 0;
	uint32_t xy[2] = { 0, 0 }, spot[2] = { 0, 0 };
	D2_CALL(D2Retail113_Ord10141_UnitCoords, unit, Addr(xy));
	const uint32_t room = D2_CALL(D2Retail113_E930, D2_CALL(D2Retail113_Ord10331_UnitRoom, unit), Addr(spot), 1, Addr(xy));
	if (!room) return 0;
	DropBlock d;
	d.W16(0x2A, Rd16(game, 0x78));
	d.W32(0x14, (uint32_t)nIndex);
	d.W32(0x24, room);
	d.W32(0x1C, spot[0]);
	d.W32(0x00, unit);
	d.W32(0x08, game);
	d.W32(0x18, 3);
	d.W32(0x20, spot[1]);
	d.W16(0x28, 1);
	d.W32(0x30, a3);
	d.W32(0x0C, D2_CALL(D2Retail113_E5D0, unit, 0));
	d.W32(0x40, a4);
	const uint32_t f = (unit && Rd32(unit, 0) == 1 && Rd32(unit, 4) == 0x187) ? 1 : 0;
	d.W32(0x80, f | nFlags);
	return D2_CALL(D2Retail113_11490, game, d.Ptr(), 0);
}

// 1.13c D2Game+0x11CA0 (EAX = xy, ESI = game): item drop in a room from a single pick
int32_t D2Retail113_11CA0(uint32_t xy, uint32_t game, uint32_t room, uint32_t p2, uint32_t p3, uint32_t p4)
{
	const uint32_t lvl = D2_CALL(D2Retail113_Ord10630_RoomLevel, room);
	int32_t ilvl = 1;
	if (lvl && !D2_CALL(D2Retail113_A808, room))
	{
		ilvl = D2_CALL(D2Retail113_ADA8, lvl, Rd8(game, 0x6D), Rd32(game, 0x70));
		if (ilvl > 1) --ilvl;
	}
	const uint32_t field = room ? room + 0x6C : 0;
	const int32_t flag = (p4 && Rd32(p4) == 1) ? 1 : 0;
	const int32_t item = D2_CALL(D2Retail113_F780, game, field, ilvl, p2, p3, flag);
	uint32_t org[2] = { Rd32(xy, 0), Rd32(xy, 4) }, in[2];
	NudgedPoint(room, Addr(org), Addr(in));
	const uint32_t at = D2_CALL(D2Retail113_AE98, room, Addr(in), Addr(org), 1, 0x3E01, 0x801, 1);
	if (!at) return 0;
	DropBlock d;
	d.W32(0x00, p4);
	d.W16(0x2A, Rd16(game, 0x78));
	d.W32(0x04, field);
	d.W32(0x1C, in[0]);
	d.W32(0x14, (uint32_t)item);
	d.W32(0x24, at);
	d.W32(0x08, game);
	d.W32(0x18, 3);
	d.W32(0x20, in[1]);
	d.W16(0x28, 1);
	d.W32(0x30, 0);
	d.W32(0x0C, (uint32_t)ilvl);
	return D2_CALL(D2Retail113_11490, game, d.Ptr(), 0);
}

// 1.13c D2Game+0x11E20 (ESI = game): item drop in a room from a pick retried up to six times
int32_t D2Retail113_11E20(uint32_t game, uint32_t room, uint32_t xy, uint32_t p3, uint32_t p4, uint32_t p5)
{
	const int32_t ilvl = RoomItemLevel(game, room);
	const uint32_t field = room ? room + 0x6C : 0;
	int32_t item;
	int tries = 0;
	do
	{
		item = D2_CALL(D2Retail113_F8F0, p3, game, field, ilvl, p4);
		if (item >= 0 && D2_CALL(D2Retail113_B1B6, item)) break;
	}
	while (++tries < 6);
	if (item < 0) return 0;
	uint32_t org[2] = { Rd32(xy, 0), Rd32(xy, 4) }, in[2];
	NudgedPoint(room, Addr(org), Addr(in));
	const uint32_t at = D2_CALL(D2Retail113_AE98, room, Addr(in), Addr(org), 1, 0x3E01, 0x801, 1);
	if (!at) return 0;
	DropBlock d;
	d.W32(0x00, p5);
	d.W32(0x04, field);
	d.W16(0x2A, Rd16(game, 0x78));
	d.W32(0x24, at);
	d.W32(0x14, (uint32_t)item);
	d.W32(0x1C, in[0]);
	d.W32(0x08, game);
	d.W32(0x18, 3);
	d.W32(0x20, in[1]);
	d.W16(0x28, 1);
	d.W32(0x30, 0);
	d.W32(0x0C, (uint32_t)ilvl);
	d.W32(0x80, 0x40);
	return D2_CALL(D2Retail113_11490, game, d.Ptr(), 0);
}

// 1.13c D2Game+0x11FB0 (EAX = xy, ECX = room, EBX = game): item drop in a room at a level-scaled quality
int32_t D2Retail113_11FB0(uint32_t xy, uint32_t room, uint32_t game, uint32_t p1, uint32_t p2, uint32_t p3)
{
	const int32_t ilvl = RoomItemLevel(game, room);
	const uint32_t field = room ? room + 0x6C : 0;
	const int32_t item = D2_CALL(D2Retail113_FA30, p1, game, field, ilvl, p2);
	if (item < 0) return 0;
	uint32_t in[2] = { Rd32(xy, 0), Rd32(xy, 4) }, spot[2] = { 0, 0 };
	const uint32_t at = D2_CALL(D2Retail113_E930, room, Addr(spot), 1, Addr(in));
	if (!at) return 0;
	DropBlock d;
	d.W32(0x00, p3);
	d.W32(0x04, field);
	d.W16(0x2A, Rd16(game, 0x78));
	d.W32(0x24, at);
	d.W32(0x14, (uint32_t)item);
	d.W32(0x1C, spot[0]);
	d.W32(0x08, game);
	d.W32(0x18, 3);
	d.W32(0x20, spot[1]);
	d.W16(0x28, 1);
	d.W32(0x30, 0);
	d.W32(0x0C, (uint32_t)ilvl);
	d.W32(0x80, 0x40);
	return D2_CALL(D2Retail113_11490, game, d.Ptr(), 0);
}

// 1.13c D2Game+0x12230 (EAX = xy): item drop in a room at a level-scaled item level capped at 64; the block is copied to out when given
int32_t D2Retail113_12230(uint32_t xy, uint32_t game, uint32_t room, uint32_t out)
{
	int32_t ilvl = RoomItemLevel(game, room);
	if (ilvl >= 0x41) ilvl = 0x40;
	const uint32_t field = room ? room + 0x6C : 0;
	const int32_t item = D2_CALL(D2Retail113_FFC0, ilvl, game, -1, 0, 0);
	uint32_t org[2] = { Rd32(xy, 0), Rd32(xy, 4) }, in[2];
	NudgedPoint(room, Addr(org), Addr(in));
	const uint32_t at = D2_CALL(D2Retail113_AE98, room, Addr(in), Addr(org), 1, 0x3E01, 0x801, 1);
	if (!at) return 0;
	DropBlock d;
	d.W16(0x2A, Rd16(game, 0x78));
	d.W32(0x14, (uint32_t)item);
	d.W32(0x1C, in[0]);
	d.W32(0x20, in[1]);
	d.W32(0x24, at);
	d.W32(0x00, 0);
	d.W32(0x04, field);
	d.W32(0x08, game);
	d.W32(0x18, 3);
	d.W16(0x28, 1);
	d.W32(0x30, 0);
	d.W32(0x0C, (uint32_t)ilvl);
	const int32_t r = D2_CALL(D2Retail113_11490, game, d.Ptr(), 0);
	if (out) std::memcpy((uint8_t*)(uintptr_t)out, d.b, sizeof d.b);
	return r;
}
