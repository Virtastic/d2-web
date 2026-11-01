// 1.13c D2Common+0x0ECB0: pick the preset index for a maze room (EAX = index or -1, EBX = level, stack R; ret 4). Raw retail 32-bit layout.
// Level: +0x1C0 type, +0x1C4 seed (8 bytes), +0x1CC list of per-definition counters {+0 id, +4 size, +8 cursor, +0xC next}. R: +4 index (out), +8 definition {+0 id, +0x40 size}.
// A definition id inside (base, base+0x10) of the level type's table gets a counter node (allocated on first use, cursor seeded by a random draw below size) and
// the room takes the next cursor value modulo size. Callees as an env: 0x9222 (allocator thunk; ECX = the pool at [[level+0x1B4]+0x478], EDX = size 0x10, stack file, line, 0) and 0x1100 (ECX = &seed, ESI = bound).
#include <cstdint>
#include <emscripten.h>

struct D2CommonECB0Env
{
	uint32_t (*alloc)(uint32_t pPoolECX, uint32_t nSizeEDX, uint32_t pFile, uint32_t nLine, uint32_t nZero);
	uint32_t (*rand_below)(uint32_t pSeedECX, uint32_t nBoundESI);
};

namespace { inline uint32_t& D(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); } }

extern "C"
void d2common_ecb0_port(uint32_t nIndexEAX, uint32_t pLevelEBX, uint32_t pRoom, const D2CommonECB0Env* pEnv)
{
	if (nIndexEAX != 0xFFFFFFFFu)
	{
		D(pRoom + 4) = nIndexEAX;
		return;
	}
	const uint32_t pDef = D(pRoom + 8);
	const int32_t nSize = static_cast<int32_t>(D(pDef + 0x40));
	const int32_t nId = static_cast<int32_t>(D(pDef));
	int32_t nBase;
	switch (D(pLevelEBX + 0x1C0))
	{
	case 3: nBase = 0x34; break;
	case 4: nBase = 0x6C; break;
	case 7: nBase = 0xA7; break;
	case 8: nBase = 0xCD; break;
	case 10: nBase = 0x101; break;
	case 0xD: nBase = 0x12D; break;
	case 0x11: nBase = 0x19D; break;
	case 0x12: nBase = 0x1E1; break;
	case 0x16: nBase = 0x2F1; break;
	case 0x18: nBase = 0x298; break;
	case 0x19: nBase = 0x2C0; break;
	case 0x1C: nBase = 0x344; break;
	case 0x21: nBase = 0x3EA; break;
	case 0x22: nBase = 0x422; break;
	case 0x23: nBase = 0x41C; break;
	default: return;
	}
	if (!(nBase < nId && nId < nBase + 0x10))
		return;
	uint32_t pNode = D(pLevelEBX + 0x1CC);
	while (pNode && static_cast<int32_t>(D(pNode)) != nId)
		pNode = D(pNode + 0xC);
	if (!pNode)
	{
		pNode = pEnv->alloc(D(D(pLevelEBX + 0x1B4) + 0x478), 0x10, 0xB2096D48u, 0x780, 0);
		D(pNode) = 0; D(pNode + 4) = 0; D(pNode + 8) = 0; D(pNode + 0xC) = 0;
		D(pNode) = static_cast<uint32_t>(nId);
		D(pNode + 4) = static_cast<uint32_t>(nSize);
		D(pNode + 8) = pEnv->rand_below(pLevelEBX + 0x1C4, static_cast<uint32_t>(nSize));
		D(pNode + 0xC) = D(pLevelEBX + 0x1CC);
		D(pLevelEBX + 0x1CC) = pNode;
	}
	const int32_t nNext = (static_cast<int32_t>(D(pNode + 8)) + 1) % static_cast<int32_t>(D(pNode + 4));
	D(pNode + 8) = static_cast<uint32_t>(nNext);
	D(pRoom + 4) = static_cast<uint32_t>(nNext);
}
