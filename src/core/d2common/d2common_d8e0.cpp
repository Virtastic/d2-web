// 1.13c D2Common+0x0D8E0: one movement step of a path (EAX = path; stack out[2], pOut; ret 8). Raw retail 32-bit layout.
// The three callees arrive as an env: 0xD0A0 (step delta), 0xD580 (tile crossing check, ESI = path), 0xD820 (unit path step).
#include <cstdint>
#include <emscripten.h>

struct D2CommonD8E0Env
{
	int32_t (*delta)(void* pPath, int32_t* pDelta2);                                         // 0xD0A0
	int32_t (*cross)(void* pPath, int32_t dx, int32_t dy, int32_t* pOut2, uint32_t* pOut);   // 0xD580
	int32_t (*unit_step)(uint32_t pUnit, int32_t nFlag);                                     // 0xD820
};

namespace
{
	inline uint32_t& D(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

extern "C"
int32_t d2common_d8e0_port(void* pPathEAX, int32_t* pOut2, uint32_t* pOut, const D2CommonD8E0Env* pEnv)
{
	uint8_t* path = static_cast<uint8_t*>(pPathEAX);
	const uint32_t pUnit = D(path, 0x30);
	*reinterpret_cast<uint16_t*>(path + 0x54) = 0;
	*pOut = 0;
	const bool bUnit = pUnit && D(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(pUnit)), 0) == 1 && (D(path, 0x34) & 0x10);
	if (D(path, 0x72) == 0 && D(path, 0x76) == 0) {
		D(path, 0x28) = 0;
		D(path, 0x24) = 0;
		pOut2[0] = static_cast<int32_t>((D(path, 0) & 0xFFFF0000u) + 0x8000u);
		pOut2[1] = static_cast<int32_t>((D(path, 4) & 0xFFFF0000u) + 0x8000u);
		return 0;
	}
	int32_t d[2];
	const int32_t r4 = pEnv->delta(path, d);
	const uint32_t nx = static_cast<uint32_t>(d[0]) + D(path, 0), ny = static_cast<uint32_t>(d[1]) + D(path, 4);
	if (static_cast<int16_t>(D(path, 0) >> 16) != static_cast<int16_t>(nx >> 16) || static_cast<int16_t>(D(path, 4) >> 16) != static_cast<int16_t>(ny >> 16)) {
		const uint32_t nMode = D(path, 0x3C);
		if (path[0x90] != 0 && nMode != 8 && nMode != 0xB)
			path[0x90] = static_cast<uint8_t>(path[0x90] - 1);
		if (pEnv->cross(path, d[0], d[1], pOut2, pOut)) {
			if (!bUnit) {
				D(path, 0x24) = D(path, 0x28);
				return 0;
			}
			return pEnv->unit_step(pUnit, D(path, 0x58) != 0) == 0 ? 1 : 0;
		}
	}
	pOut2[0] = static_cast<int32_t>(nx);
	pOut2[1] = static_cast<int32_t>(ny);
	if (!r4)
		return 0;
	D(path, 0x24) += 1;
	return 1;
}
