// 1.13c D2Common+0x10EB0 (stdcall, ret 4): place the level's linked area against the room the record's kind selects. Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_10eb0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x10EB0
{
	const uint32_t s = FindRec(e, D(pLevel + 0x1B4), 0x1B);
	int32_t kind = static_cast<int32_t>(D(D(s + 0x14) + 4));
	uint32_t room;
	if (kind == 0)
		room = e->f10820(pLevel);
	else if (kind == 1)
		room = e->f10730(pLevel);
	else if (kind == 2)
		room = e->f10910(pLevel);
	else
		return; // retail: indexes past the slot table
	int32_t x = static_cast<int32_t>(D(s + 0x1C));
	int32_t y = static_cast<int32_t>(D(s + 0x20));
	const uint32_t P = D(pLevel + 0x14);
	const uint32_t dir = kind == 0 ? 2u : kind == 1 ? 3u : 0u;
	const uint32_t p = e->f000(room, dir, 0);
	if (p)
	{
		e->pick(room, 1);
		Tag(p, 0xA7, static_cast<uint32_t>(kind));
	}
	e->f44f20(s, p, dir, 0);
	const int32_t px = static_cast<int32_t>(D(p + 0x34)), py = static_cast<int32_t>(D(p + 0x38));
	if (kind == 0)
	{
		x -= px + static_cast<int32_t>(D(P + 0x10));
		y += Sdiv2(static_cast<int32_t>(D(s + 0x28))) - py;
	}
	else if (kind == 1)
	{
		x += Sdiv2(static_cast<int32_t>(D(s + 0x24))) - px - 6;
		y -= py + static_cast<int32_t>(D(P + 0x14));
	}
	else
	{
		x += static_cast<int32_t>(D(s + 0x24)) - px;
		y += Sdiv2(static_cast<int32_t>(D(s + 0x28))) - py + 1;
	}
	int32_t a = kind;
	if ((Lcg(pLevel) & 1u) == 0)
	{
		e->f106b0(At(0x96138, a), pLevel, &a);
		e->f106b0(At(0x960F8, a), pLevel, &a);
	}
	else
	{
		e->f106b0(At(0x960F8, a), pLevel, &a);
		e->f106b0(At(0x96138, a), pLevel, &a);
	}
	for (uint32_t r = D(pLevel + 0x10); r; r = D(r + 0x24))
	{
		D(r + 0x34) += static_cast<uint32_t>(x);
		D(r + 0x38) += static_cast<uint32_t>(y);
	}
	Bounds(pLevel);
}
