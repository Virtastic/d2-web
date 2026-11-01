// 1.13c D2Common+0x0EB80: for each other room that touches this one and is not yet linked, roll a per-room LCG and maybe link them (stack P; ret 4). Raw retail 32-bit layout.
// P/Q (rooms): +0 first link {+0 other room, +0x14 next}, +0x14/+0x18 LCG state (low, high), +0x20 definition (+0xC bit 1 = disabled), +0x24 next room,
// +0x34/+0x38 x, y, +0x3C/+0x40 w, h, +0x58 level data {+0x10 first room, +0x14 settings {+0x18 percent}}.
// Callees as an env: 0x2D470 (EAX = settings, ECX = 1000, EDX = &P+0x34, ESI = &Q+0x34; -1 = no link), 0x44BA0 (EAX = that result, ECX = Q, EBX = P), 0xE490 (EBX = Q, stack 1).
#include <cstdint>
#include <emscripten.h>

struct D2CommonEB80Env
{
	int32_t (*pick)(uint32_t pSettingsEAX, uint32_t nThousandECX, uint32_t pPRectEDX, uint32_t pQRectESI);
	void (*link)(uint32_t nPickEAX, uint32_t pQECX, uint32_t pPEBX);
	void (*e490)(uint32_t pQEBX, uint32_t nArg);
};

namespace { inline uint32_t& D(uint32_t a) { return *reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(a)); } }

extern "C"
void d2common_eb80_port(uint32_t pP, const D2CommonEB80Env* pEnv)
{
	if (((D(D(pP + 0x20) + 0xC) >> 1) & 1))
		return;
	const uint32_t pLevel = D(pP + 0x58);
	const uint32_t pSettings = D(pLevel + 0x14);
	for (uint32_t pQ = D(pLevel + 0x10); pQ; pQ = D(pQ + 0x24))
	{
		if (pQ == pP || ((D(D(pQ + 0x20) + 0xC) >> 1) & 1))
			continue;
		const int32_t px = D(pP + 0x34), qx = D(pQ + 0x34);
		const int32_t gapX = px < qx ? (qx - static_cast<int32_t>(D(pP + 0x3C))) - px : (px - static_cast<int32_t>(D(pQ + 0x3C))) - qx;
		const int32_t py = D(pP + 0x38), qy = D(pQ + 0x38);
		const int32_t gapY = py < qy ? (qy - static_cast<int32_t>(D(pP + 0x40))) - py : (py - static_cast<int32_t>(D(pQ + 0x40))) - qy;
		if (gapX >= 1 || gapY >= 1 || gapX == gapY)
			continue;
		bool bLinked = false;
		for (uint32_t pL = D(pP); pL; pL = D(pL + 0x14))
			if (D(pL) == pQ) { bLinked = true; break; }
		if (bLinked)
			continue;
		const uint64_t nState = static_cast<uint64_t>(D(pQ + 0x14)) * 0x6AC690C5ull + D(pQ + 0x18);
		D(pQ + 0x14) = static_cast<uint32_t>(nState);
		D(pQ + 0x18) = static_cast<uint32_t>(nState >> 32);
		if (static_cast<int32_t>(static_cast<uint32_t>(nState) % 1000) < static_cast<int32_t>(D(pSettings + 0x18)))
		{
			const int32_t nPick = pEnv->pick(pSettings, 1000, pP + 0x34, pQ + 0x34);
			if (nPick != -1)
			{
				pEnv->link(static_cast<uint32_t>(nPick), pQ, pP);
				pEnv->e490(pQ, 1);
			}
		}
	}
}
