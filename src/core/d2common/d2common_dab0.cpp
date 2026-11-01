// 1.13c D2Common+0x0DAB0: sync a rider unit's path to its mount's (stdcall rider, mount; ret 8). Raw retail 32-bit layout;
// the room recache at 0xDA40 (ESI = rider path; stack x, y, room; ret 0xC) is the env. A null rider or mount is fatal in retail
// (not modelled: the call never returns there).
#include <cstdint>
#include <emscripten.h>

struct D2CommonDAB0Env
{
	void (*recache)(uint32_t pPath, uint32_t nX, uint32_t nY, uint32_t pRoom);   // 0xDA40
};

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& D(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint16_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint16_t*>(b + o); }
}

extern "C"
void d2common_dab0_port(uint32_t pRider, uint32_t pMount, const D2CommonDAB0Env* pEnv)
{
	uint8_t* rider = P(D(P(pRider), 0x2C));
	uint8_t* mount = P(D(P(pMount), 0x2C));
	if (W(rider, 2) == W(mount, 2) && W(rider, 6) == W(mount, 6))
		D(rider, 0x34) &= ~8u;
	else
		D(rider, 0x34) |= 8u;
	if (D(rider, 0x1C) != D(mount, 0x1C))
		D(rider, 0x34) |= 1u;
	pEnv->recache(static_cast<uint32_t>(reinterpret_cast<uintptr_t>(rider)), D(mount, 0), D(mount, 4), D(mount, 0x1C));
}
