// 1.13c D2Common+0x0DCE0: the dynamic-path move step (stdcall path, unit, room, tx, ty; ret 0x14). Raw retail 32-bit layout.
// Callees as an env: 0x4D5C0 (collision set, 7 stack words), 0xDA40 (ESI = path; stack x, y, room), 0x1330 (EAX = room; EBX = tx; EDI = ty), 0xDC80 (EAX = path).
// The two fatal exits (unit type neither 0 nor 1: 0x50D; destination room lookup 0: 0x524) never return in retail and return 0 here.
#include <cstdint>
#include <emscripten.h>

struct D2CommonDCE0Env
{
	uint32_t (*collide)(uint32_t nX, uint32_t nY, uint32_t pRoom, int32_t nTx, int32_t nTy, uint32_t nMask1, uint32_t nMask2);   // 0x4D5C0
	void (*recache)(uint32_t pPath, uint32_t nX, uint32_t nY, uint32_t pRoom);                                                   // 0xDA40
	uint32_t (*get_room)(uint32_t pRoom, uint32_t nTx, uint32_t nTy);                                                            // 0x1330
	void (*finish)(uint32_t pPath);                                                                                              // 0xDC80
};

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& D(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint16_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint16_t*>(b + o); }
}

extern "C"
int32_t d2common_dce0_port(uint32_t pPath, uint32_t pUnit, uint32_t pRoom, int32_t nTx, int32_t nTy, const D2CommonDCE0Env* pEnv)
{
	uint8_t* path = P(pPath);
	if (!pUnit || (D(P(pUnit), 0) != 0 && D(P(pUnit), 0) != 1))
		return 0;
	if (pEnv->collide(W(path, 2), W(path, 6), pRoom, nTx, nTy, D(path, 0x4C), D(path, 0x50)))
		return 0;
	if (D(path, 0x1C) != pRoom)
		D(path, 0x34) |= 1;
	pEnv->recache(pPath, static_cast<uint32_t>(nTx) * 0x10000u + 0x8000u, static_cast<uint32_t>(nTy) * 0x10000u + 0x8000u, pRoom);
	if (!pEnv->get_room(pRoom, nTx, nTy))
		return 0;
	pEnv->finish(pPath);
	return 1;
}
