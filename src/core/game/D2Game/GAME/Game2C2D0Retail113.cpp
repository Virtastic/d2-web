#include <D2Game/GAME/Game2C2D0Retail113.h>
#include <test_seam.h>
#include <cstring>

// Retail-layout GAME_CreateNewEmptyGame. Convention: EAX = flags, 11 stack words a[0..10]; ret 0x2C; no result. a[0] the client id,
// a[1] the game name, a[2] / a[6] / a[7] bytes stored at +0x6A / +0x6B / +0x6D, a[3] a[4] a[8] go to the first client.
// Game (retail layout, 32-bit words): +0x18 lock, +0x1C, +0x20 server cookie, +0x24, +0x28 game id, +0x6A..+0x6D bytes, +0x70 / +0x74
// flag bits, +0x78 word, +0x80, +0x88 / +0x8C client list and count, +0x90..+0xA4, six 0x200-byte tables, +0x1DC4. Client: +0x1AC byte.
uint32_t D2Retail113_GameCall(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_GameEdi(uint32_t, uint32_t) { __builtin_trap(); }
void D2Retail113_GameSetHooks(uint32_t (*call)(uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t),
                              void (*edi)(uint32_t, uint32_t))
{
	D2Seam<&D2Retail113_GameCall>::hook = call;
	D2Seam<&D2Retail113_GameEdi>::hook = edi;
}

namespace
{
inline uint32_t rd(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline void wr(uint32_t a, uint32_t v) { std::memcpy((void*)(uintptr_t)a, &v, 4); }
inline void wr8(uint32_t a, uint32_t v) { *(uint8_t*)(uintptr_t)a = (uint8_t)v; }
inline void wr16(uint32_t a, uint32_t v) { const uint16_t w = (uint16_t)v; std::memcpy((void*)(uintptr_t)a, &w, 2); }

uint32_t Call(uint32_t rva, uint32_t a = 0, uint32_t c = 0, uint32_t d = 0, uint32_t b = 0, uint32_t s = 0, uint32_t s0 = 0, uint32_t s1 = 0,
              uint32_t s2 = 0, uint32_t s3 = 0)
{
	return D2_CALL(D2Retail113_GameCall, rva, a, c, d, b, s, s0, s1, s2, s3);
}
void Edi(uint32_t rva, uint32_t v) { D2_CALL(D2Retail113_GameEdi, rva, v); }

constexpr uint32_t kFogFile = 0x6FD1CD98u;    // the Fog source-file string handed to both allocations
constexpr uint32_t kModuleName = 0x6FD19FF9u; // the module-name literal handed to the client add
}

// 1.13c D2Game+0x2C2D0
void D2Retail113_2C2D0(uint32_t flags, const uint32_t* a, uint32_t srvCell, uint32_t cbCell, uint32_t diffCell)
{
	const uint32_t srv = rd(srvCell);
	if (!srv) return;
	const uint32_t key = Call(0x2060);
	Edi(0x2060, srv);
	const uint32_t g = Call(0x2090, 0, 0, 0, 0, 0, key, 0, 0);
	Edi(0x2090, srv);
	if (!g) return;
	wr(g + 0x18, Call(0xA2E6, 0, 0, 0x18, 0, 0, kFogFile, 0x4A6, 0));
	wr(g + 0x1C, 0);
	static const uint32_t kTables[6] = { 0x1120, 0x1320, 0x1520, 0x1720, 0x1920, 0x1B24 };
	for (uint32_t o : kTables) std::memset((void*)(uintptr_t)(g + o), 0, 0x200);
	wr(g + 0x24, 1);
	wr16(g + 0x28, Call(0x2A220));
	Call(0x2BC90, g);
	Edi(0x2BC90, a[1]);
	wr8(g + 0x6A, a[2]);
	for (uint32_t o = 0x90; o <= 0xA4; o += 4) wr(g + o, 0);
	wr8(g + 0x6D, a[7]);
	wr8(g + 0x6B, a[6]);
	wr(g + 0x70, (flags & 0x100000) ? 1 : 0);
	wr16(g + 0x78, rd(g + 0x70) ? 0x65 : 2);
	wr(g + 0x74, flags >> 21 & 1);
	Call(0x2A900);
	Edi(0x2A900, g);
	Call(0x8E630, 0, 0, 0, g);
	Call(0xB2560, flags, 0, 0, 0, 0, a[6]);
	Edi(0xB2560, g);
	wr(g + 0x8C, 0);
	wr(g + 0x88, 0);
	if (const uint32_t t = rd(cbCell)) if (const uint32_t fn = rd(t + 0x34)) wr(g + 0x20, Call(kD2Retail113CookieCall, 0, 0, 0, 0, 0, fn));
	const uint32_t c = Call(0x4A9B0, a[0], 0, 0, 0, 0, g, a[3], a[4], kModuleName);
	Call(0xA38E, 0, 0, 0, 0, 0, a[0], key);
	if (*(const uint8_t*)(uintptr_t)(g + 0x6A) == 1) Call(0x49580, c);
	Call(0x9BE40, 0, 0, 0, 0, g);
	Call(0xCACC0, 0, 0, 0, 0, g);
	wr(g + 0x80, Call(0xB9A0, 0, 0, 0, 0, 0, g));
	Call(0x8FBE0, 0, 0, 0, 0, 0, g);
	Call(0x348A0, 0, 0, 0, 0, 0, g);
	Call(0x2A2D0, 0, 0, 0, 0, 0, key, rd(g + 0x28) & 0xFFFF);
	Call(0x2AC60, g, 0, 0, 0, c);
	if (!c) { Call(0xA2DA, 0, 0, 0, 0, 0, 0x7D6); return; }
	const uint32_t d = *(const uint8_t*)(uintptr_t)diffCell;
	if (d >= 5) { Call(0xA2DA, 0, 0, 0, 0, 0, 0x7D7); return; }
	wr8(c + 0x1AC, d);
	const uint32_t n = Call(0xA2F2);
	wr(g + 0x1DC4, n > 1 ? n : 2);
	const uint32_t p = Call(0xA34C, 0, 0x14, kFogFile, 0, 0, 0x524, 0);
	Call(0x52390, 0, p, 0, 0, 0, key);
	if (const uint32_t t = rd(cbCell)) if (rd(t + 8)) { Call(0xA2DA, 0, 0, 0, 0, 0, 0x52B); return; }
	// retail reuses a[2]'s stack slot as the one-byte packet: low byte 2, the upper bytes stay a[2]'s
	uint32_t slot = (a[2] & 0xFFFFFF00u) | 2u;
	Call(0x8A3E0, c, 0, 0, 0, 0, (uint32_t)(uintptr_t)&slot, 1);
}
