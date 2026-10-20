// Prior art used: D2MOO 1.10f AiThink.cpp, checked against cached retail 1.13c
// decompiles. AI handlers reuse the matching D2MOO handler; the packet helpers take
// the register-passed client explicitly.
#include <cstdint>
#include <cstring>
#include "AI/AiThink.h"
#include "GAME/SCmd.h"

// 1.13c D2Game+0x83CE0 (Brute)
extern "C" void d2game_83ce0(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* p) { AITHINK_Fn007_Brute(g, u, p); }

// 1.13c D2Game+0x851E0 (SkeletonMage)
extern "C" void d2game_851e0(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* p) { AITHINK_Fn064_SkeletonMage(g, u, p); }

// 1.13c D2Game+0x85B60 (Mephisto)
extern "C" void d2game_85b60(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* p) { AITHINK_Fn050_Mephisto(g, u, p); }

// 1.13c D2Game+0x87A60 (SandMaggotQueen)
extern "C" void d2game_87a60(D2GameStrc* g, D2UnitStrc* u, D2AiTickParamStrc* p) { AITHINK_Fn066_SandMaggotQueen(g, u, p); }

// 1.13c D2Game+0x8A330: hysteresis on a value; ctx+4 last int, ctx+0xc byte counter
extern "C" int d2game_8a330(uint8_t* ctx, int v)
{
	int* last = (int*)(ctx + 4);
	uint8_t* cnt = ctx + 0xc;
	int d = v - *last;
	if (d == 0) return 0;
	if (d < 0) d = -d;
	if (d > 8 || ++*cnt > 6) { *cnt = 0; *last = v; return 1; }
	return 0;
}

// 1.13c D2Game+0x8AA70: copy a 21-byte packet and send
extern "C" void d2game_8aa70(D2ClientStrc* c, const uint8_t* src)
{
	uint8_t pkt[0x15];
	memcpy(pkt, src, sizeof pkt);
	D2GAME_PACKETS_SendPacket_6FC3C710(c, pkt, 0x15);
}

// 1.13c D2Game+0x8AD10: 7-byte packet, byte1 = a, bytes2..5 = b (byte0 left unset in retail)
extern "C" void d2game_8ad10(D2ClientStrc* c, uint8_t a, uint32_t b)
{
	uint8_t pkt[8] = {};
	pkt[1] = a;
	memcpy(pkt + 2, &b, 4);
	D2GAME_PACKETS_SendPacket_6FC3C710(c, pkt, 7);
}
