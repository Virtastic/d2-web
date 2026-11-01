// 1.13c D2Common+0x0F000 / 0x0F090: allocate a maze room, link it to the level and put it at the head of the level's room list.
// F000 is stdcall (level, kind, flag; ret 0xC). F090 takes ECX = level, EAX = kind, EDI = flag, calls F000 and, on success, runs 0xE490 with EBX = the level (when EDI != 0)
// and then with EBX = the room. Raw retail 32-bit layout; callees are the env: 0x44BD0 (alloc, stack kind), 0xEEC0 (EAX = kind, ECX = level, EDX = room),
// 0x44F90 (free, stack room), 0x44950 (EAX, ESI, stack n), 0xEB80 (stack room), 0xE490 (EBX = room, stack 1).
#include <cstdint>
#include <emscripten.h>

struct D2CommonF000Env
{
	uint32_t (*alloc)(uint32_t nKind);
	int32_t (*link)(uint32_t nKindEAX, uint32_t pLevelECX, uint32_t pRoomEDX);
	void (*free_room)(uint32_t pRoom);
	void (*adjust)(uint32_t pEAX, uint32_t pESI, uint32_t nArg);
	void (*scatter)(uint32_t pRoom);
	void (*e490)(uint32_t pRoomEBX, uint32_t nArg);
};

namespace
{
	inline uint32_t& D(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); }
}

extern "C"
uint32_t d2common_f000_port(uint32_t pLevel, uint32_t nKind, uint32_t nFlag, const D2CommonF000Env* pEnv)
{
	const uint32_t pRoom = pEnv->alloc(2);
	const uint32_t pDef = D(D(pRoom + 0x58) + 0x14);
	D(pRoom + 0x3C) = D(pDef + 0x10);
	D(pRoom + 0x40) = D(pDef + 0x14);
	if (!pEnv->link(nKind, pLevel, pRoom))
	{
		pEnv->free_room(pRoom);
		return 0;
	}
	pEnv->adjust(pRoom, pLevel, nKind);
	pEnv->adjust(pLevel, pRoom, (nKind - 2) & 3);
	if (nFlag)
		pEnv->scatter(pRoom);
	const uint32_t pList = D(pLevel + 0x58);
	D(pRoom + 0x24) = D(pList + 0x10);
	D(pList + 8) += 1;
	D(pList + 0x10) = pRoom;
	return pRoom;
}

extern "C"
uint32_t d2common_f090_port(uint32_t pLevelECX, uint32_t nKindEAX, uint32_t nFlagEDI, const D2CommonF000Env* pEnv)
{
	const uint32_t pRoom = d2common_f000_port(pLevelECX, nKindEAX, nFlagEDI, pEnv);
	if (pRoom)
	{
		if (nFlagEDI)
			pEnv->e490(pLevelECX, 1);   // retail: EBX still holds the level here

		pEnv->e490(pRoom, 1);
	}
	return pRoom;
}
