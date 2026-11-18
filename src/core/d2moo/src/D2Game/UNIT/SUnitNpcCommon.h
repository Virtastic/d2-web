#pragma once
// Shared by the retail-layout NPC bodies (C0DB0, BDB10, C1250): field access on 32-bit retail addresses, the D2_CALL callee stub
// (callees keyed by RVA through D2Retail113_NpcCall / NpcEdi) and the 0x2A reply senders. Include after <test_seam.h> and NpcRetail113.h.
#include <cstdint>
#include <cstring>

namespace npc113
{
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline uint32_t rd16(uint32_t a) { uint16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
inline uint32_t rd8(uint32_t a) { return *(const uint8_t*)(uintptr_t)a; }
inline void wr32(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
inline void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }

inline uint32_t Call(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t s0 = 0,
                     uint32_t s1 = 0, uint32_t s2 = 0, uint32_t s3 = 0, uint32_t s4 = 0, uint32_t s5 = 0, uint32_t s6 = 0, uint32_t s7 = 0)
{
	return D2_CALL(D2Retail113_NpcCall, rva, a, c, d, b, s, s0, s1, s2, s3, s4, s5, s6, s7);
}
// REGS `J`: EDI handed to the callee just called
inline void Edi(uint32_t rva, uint32_t v) { D2_CALL(D2Retail113_NpcEdi, rva, v); }

// E03A0 (when player+0x6c is set) names the unit the player is talking to; the game's node table at +0x1320 (by guid & 0x7f) must
// hold a node of that guid that IS that unit, else an inline 15-byte packet 2A,00,09 is sent. 8AC90 is the 0x2A reply sender
// (AL = 0x2A, CL = code, EDX = the 10973 result, stack: client, id/-1, 0). The game == 0 abort of the original is not reproduced.
inline uint32_t ClientOf(uint32_t player)
{
	if (rd32(player) != 0) return 0;
	return rd32(Call(0xA4C0, 0, 0, 0, 0, 0, player) + 0x9c);
}
inline void Reply2A(uint32_t player, uint32_t code, uint32_t id)
{
	const uint32_t client = ClientOf(player);
	const uint32_t v = Call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
	Call(0x8AC90, 0x2A, code, v, 0, 0, client, id, 0);
}
inline void Packet15(uint32_t player)
{
	const uint32_t client = ClientOf(player);
	const uint32_t v = Call(0xA42A, 0, 0, 0, 0, 0, player, 0xE, 0);
	uint8_t b[15] = { 0x2A, 0, 9 };
	const uint32_t m1 = 0xFFFFFFFFu;
	std::memcpy(b + 7, &m1, 4); std::memcpy(b + 11, &v, 4);
	Call(0x8A3E0, client, 0, 0, 0, 0, (uint32_t)(uintptr_t)b, 0xF);
}
}
