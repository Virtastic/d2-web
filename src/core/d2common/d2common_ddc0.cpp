// 1.13c D2Common+0x0DDC0: move a path's collision footprint to a new room/position (EAX = path; stack pRoom, packed x|y<<16; ret 8).
// Raw retail 32-bit layout. Callees as an env: 0x4BCF0 / 0x4BE90 (unit footprint clear / set), 0x4BE10 / 0x4BFC0 (missile footprint clear / set),
// all (room, x, y, a, b) stdcall; 0xDA40 (ESI = path; x, y, room); 0x1330 (EAX = room; EBX = x; EDI = y); 0xDC80 (EAX = path).
// The room lookup returning 0 is fatal (0x4CC) in retail and returns 0 here.
#include <cstdint>
#include <emscripten.h>

struct D2CommonDDC0Env
{
	void (*clear_unit)(uint32_t pRoom, uint32_t nX, uint32_t nY, uint32_t nA, uint32_t nB);   // 0x4BCF0
	void (*set_unit)(uint32_t pRoom, uint32_t nX, uint32_t nY, uint32_t nA, uint32_t nB);     // 0x4BE90
	void (*clear_missile)(uint32_t pRoom, uint32_t nX, uint32_t nY, uint32_t nA, uint32_t nB);// 0x4BE10
	void (*set_missile)(uint32_t pRoom, uint32_t nX, uint32_t nY, uint32_t nA, uint32_t nB);  // 0x4BFC0
	void (*recache)(uint32_t pPath, uint32_t nX, uint32_t nY, uint32_t pRoom);                // 0xDA40
	uint32_t (*get_room)(uint32_t pRoom, uint32_t nX, uint32_t nY);                           // 0x1330
	void (*finish)(uint32_t pPath);                                                           // 0xDC80
};

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& D(uint8_t* b, int o) { return *reinterpret_cast<uint32_t*>(b + o); }
	inline uint16_t& W(uint8_t* b, int o) { return *reinterpret_cast<uint16_t*>(b + o); }
}

extern "C"
int32_t d2common_ddc0_port(void* pPathEAX, uint32_t pRoom, uint32_t nPacked, const D2CommonDDC0Env* pEnv)
{
	uint8_t* path = static_cast<uint8_t*>(pPathEAX);
	const uint32_t pPathAddr = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(path));
	const uint32_t nLo = nPacked & 0xFFFF, nHi = nPacked >> 16;
	const uint32_t x = W(path, 2), y = W(path, 6), old = D(path, 0x1C);
	const uint32_t pUnit = D(path, 0x30);
	if (!pUnit || D(P(pUnit), 0) != 3) {
		if (nPacked == 0) {
			pEnv->clear_unit(old, x, y, D(path, 0x48), D(path, 0x4C));
		} else if (old) {
			pEnv->clear_unit(old, x, y, D(path, 0x48), D(path, 0x4C));
			pEnv->set_unit(pRoom, nLo, nHi, D(path, 0x48), D(path, 0x4C));
		}
	} else if (nPacked == 0) {
		pEnv->clear_missile(old, x, y, D(path, 0x44), D(path, 0x4C));
	} else {
		if (old)
			pEnv->clear_missile(old, x, y, D(path, 0x44), D(path, 0x4C));
		pEnv->set_missile(pRoom, nLo, nHi, D(path, 0x44), D(path, 0x4C));
	}
	if (D(path, 0x1C) != pRoom)
		D(path, 0x34) |= 1;
	pEnv->recache(pPathAddr, nPacked * 0x10000u + 0x8000u, (nPacked >> 16) * 0x10000u + 0x8000u, pRoom);
	if (nPacked != 0 && !pEnv->get_room(pRoom, nLo, nHi))
		return 0;
	pEnv->finish(pPathAddr);
	return 1;
}
