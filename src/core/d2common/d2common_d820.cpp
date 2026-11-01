// 1.13c D2Common+0x0D820: step the unit's path toward its target (register-free stdcall, ret 8). The engine's vendored
// counterpart works on the wasm64 unit structs; this form reads the retail 32-bit layout (blob adapters), with the two
// callees as an env: 0x665C0 (unit room-list refresh) and 0x36230 (path step: path, unit, 0).
#include <cstdint>
#include <emscripten.h>

struct D2CommonD820Env
{
	void (*refresh)(uint32_t pUnit);                                       // 0x665C0
	int32_t (*step)(uint32_t pPath, uint32_t pUnit, uint32_t nArg);        // 0x36230
};

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& D(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

extern "C"
int32_t d2common_d820_port(uint32_t pUnit, int32_t nA2, const D2CommonD820Env* pEnv)
{
	uint8_t* unit = P(pUnit);
	const uint32_t pPath = D(unit, 0x2C);
	if (!pPath)
		return 0;
	uint8_t* path = P(pPath);
	if (!(path[0x34] & 0x10)) {
		if (D(unit, 0) == 1 && path[0x94] == 0)
			return 0;
		pEnv->refresh(pUnit);
		D(unit, 0xC4) |= 1;
		const int32_t nLeft = static_cast<int32_t>(path[0x94]) - static_cast<int32_t>(D(path, 0x24));
		path[0x94] = nLeft < 0 ? 0 : (nLeft < 0x100 ? static_cast<uint8_t>(nLeft) : 0xFF);
	}
	const uint32_t nType = D(path, 0x3C);
	if (nType == 2 || nType == 0xD || nType == 0xF) {
		if (nA2 == 0) {
			D(path, 0x3C) = 2;
			D(path, 0x10) = D(path, 0x18);
		} else {
			D(path, 0x3C) = 0xD;
		}
		const int32_t nRet = pEnv->step(pPath, pUnit, 0);
		if (nRet != 0)
			return nRet;
		D(path, 0x3C) = 0xF;
	}
	return pEnv->step(pPath, pUnit, 0);
}
