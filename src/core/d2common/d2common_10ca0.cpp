// 1.13c D2Common+0x10CA0 (stdcall, ret 4): build the level's start-area preset set. Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_10ca0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x10CA0
{
	const uint32_t pOuter = D(pLevel + 0x1B4);
	if (static_cast<uint8_t>(D(pOuter + 0x480)) == 4)
		return;
	const uint32_t s = FindRec(e, pOuter, 0x6C);
	uint32_t pBase = e->f10730(pLevel);
	uint32_t p = e->f000(pBase, 3, 0);
	if (p)
	{
		e->pick(pBase, 1);
		Tag(p, 0x354, 0xFFFFFFFFu);
	}
	pBase = e->f10a00(pLevel);
	p = e->f000(pBase, 1, 0);
	if (p)
	{
		e->pick(pBase, 1);
		Tag(p, 0x357, 0xFFFFFFFFu);
	}
	const uint32_t q = e->f000(p, 1, 0);
	if (q)
		Tag(q, 0x358, 0xFFFFFFFFu);
	const uint32_t pEnd = e->f000(q, 1, 0);
	if (pEnd)
		Tag(pEnd, 0x358, 0xFFFFFFFFu);
	e->f44f20(s, pEnd, 1, 0);
	const int32_t dx = 2 * static_cast<int32_t>(D(pEnd + 0x3C)) - static_cast<int32_t>(D(pEnd + 0x34)) + static_cast<int32_t>(D(s + 0x1C));
	const int32_t dy = static_cast<int32_t>(D(s + 0x28)) - static_cast<int32_t>(D(pEnd + 0x38)) + static_cast<int32_t>(D(s + 0x20));
	const uint32_t lo = Lcg(pLevel);
	e->f106b0(Rec(0x960D8u + (lo & 1u) * 16u), pLevel, nullptr);
	e->f10010(pLevel, 0x344, pEnd);
	for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
	{
		D(r + 0x34) += static_cast<uint32_t>(dx);
		D(r + 0x38) += static_cast<uint32_t>(dy);
	}
	Bounds(pLevel);
}
