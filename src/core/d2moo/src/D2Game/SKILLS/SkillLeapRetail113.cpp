#include <test_seam.h>
#include "SKILLS/SkillLeapRetail113.h"

#include <cstring>

// Retail-layout (1.13c) leap-attack landing step, written from the 1.13c decompile. The callees trap here (the game has no
// retail-layout caller for them); the FN probe replaces them through D2Seam.
#define D2R_TRAP() __builtin_trap()
uint32_t D2Retail113_AB44(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_AD3C(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_E03A0(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_A78A(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_ACE8(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_ACEE(uint32_t) { D2R_TRAP(); }
void D2Retail113_AB14(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_AD18(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_A5FE(uint32_t, uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_1100(uint32_t) { D2R_TRAP(); }
int32_t D2Retail113_1130(uint32_t) { D2R_TRAP(); }
uint32_t D2Retail113_A478(uint32_t) { D2R_TRAP(); }
void D2Retail113_AB2C(uint32_t, int32_t, int32_t) { D2R_TRAP(); }
void D2Retail113_AAFC(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_AD24(uint32_t, uint32_t) { D2R_TRAP(); }
void D2Retail113_AAF0(uint32_t, uint32_t, uint32_t) { D2R_TRAP(); }

namespace
{
inline uint32_t Rd32(uint32_t a, uint32_t o) { uint32_t v; std::memcpy(&v, (const uint8_t*)(uintptr_t)a + o, 4); return v; }
}

// 1.13c D2Game+0x27030
uint32_t D2Retail113_27030(uint32_t skill, uint32_t game, uint32_t unit, int32_t x, int32_t y)
{
	if (!skill) return 0;
	const uint32_t path = Rd32(unit, 0x2c);
	if (!path) return 0;
	const uint32_t id = D2_CALL(D2Retail113_AB44, skill);
	const uint32_t type = D2_CALL(D2Retail113_AD3C, skill);
	const uint32_t target = D2_CALL(D2Retail113_E03A0, type, game, id);
	if (!target) return 0;
	D2_CALL(D2Retail113_A78A, skill, 1u);
	D2_CALL(D2Retail113_ACE8, unit, 0xcu);
	const uint32_t frame = (Rd32(unit, 0x48) & 0xffffff00u) + 0x100;
	std::memcpy((uint8_t*)(uintptr_t)unit + 0x48, &frame, 4);
	D2_CALL(D2Retail113_ACEE, path);
	D2_CALL(D2Retail113_AB14, path, 5u);
	D2_CALL(D2Retail113_AD18, path, 1u);
	D2_CALL(D2Retail113_A5FE, path, 0u);
	const int32_t tx = (int32_t)((uint32_t)x * 3u - (uint32_t)D2_CALL(D2Retail113_1100, target) * 2u);
	const int32_t ty = (int32_t)((uint32_t)y * 3u - (uint32_t)D2_CALL(D2Retail113_1130, target) * 2u);
	const uint32_t room = D2_CALL(D2Retail113_1280, D2_CALL(D2Retail113_A478, unit), (uint32_t)tx, (uint32_t)ty);
	if (room && D2_CALL(D2Retail113_A808, room)) return 0;
	D2_CALL(D2Retail113_AB2C, path, tx, ty);
	D2_CALL(D2Retail113_AAFC, path, 8u);
	D2_CALL(D2Retail113_AD24, path, 5u);
	D2_CALL(D2Retail113_AAF0, path, unit, 0u);
	return 1;
}
