// 1.13c D2Common+0x0F110: finish a placed maze preset (EAX = level; stack S; ret 4). Raw retail 32-bit layout.
// S: +0 first link, +0x20 definition record {+0 id, +4 seed word}, +0x34 size record {+8 w, +0xC h}. Link: +0 room, +4 dir, +0xC kind, +0x14 next.
// Callees as an env: 0xBC60 (stack level, def id, &S+0x34; EBX = level+0x1C4; returns the new room), 0xECB0 (EAX = seed word, EBX = level, stack room),
// 0xC690 (stack level, room, 0, small flag; returns a room), 0x44950 (EAX, ESI, stack n), 0x44F90 (tail call, stack S: frees it).
#include <cstdint>
#include <emscripten.h>

struct D2CommonF110Env
{
	uint32_t (*make_room)(uint32_t pLevel, uint32_t nId, uint32_t pSize, uint32_t pEBX);
	void (*seed)(uint32_t nSeedEAX, uint32_t pLevelEBX, uint32_t pRoom);
	uint32_t (*place)(uint32_t pLevel, uint32_t pRoom, uint32_t nZero, uint32_t nSmall);
	void (*adjust)(uint32_t pEAX, uint32_t pESI, uint32_t nArg);
	void (*free_s)(uint32_t pS);
};

namespace { inline uint32_t& D(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); } }

extern "C"
void d2common_f110_port(uint32_t pLevelEAX, uint32_t pS, const D2CommonF110Env* pEnv)
{
	const uint32_t pDef = D(pS + 0x20);
	const uint32_t pSize = pS + 0x34;
	const uint32_t pRoom = pEnv->make_room(pLevelEAX, D(pDef), pSize, pLevelEAX + 0x1C4);
	pEnv->seed(D(pDef + 4), pLevelEAX, pRoom);
	const uint32_t nSmall = (static_cast<int32_t>(D(pSize + 8)) <= 12 && static_cast<int32_t>(D(pSize + 0xC)) <= 12) ? 1 : 0;
	const uint32_t pNew = pEnv->place(pLevelEAX, pRoom, 0, nSmall);
	for (uint32_t pLink = D(pS); pLink; pLink = D(pLink + 0x14))
	{
		if (D(pLink + 0xC) == 1)
		{
			const uint32_t nDir = D(pLink + 4);
			pEnv->adjust(D(pLink), pNew, nDir);
			pEnv->adjust(pNew, D(pLink), (nDir - 2) & 3);
		}
	}
	pEnv->free_s(pS);
}
