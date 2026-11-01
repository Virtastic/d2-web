// 1.13c D2Common+0x116B0: two LCG rolls (a 1-or-3 parity value, a 0-3 counter); level 0x2F builds a chain of rooms with the 0x14D / 0x150 presets, 0x30 / 0x31 / 0x41 apply record pairs (ESI = level ctx; plain ret). Raw retail 32-bit layout; preset env.
#include <cstdint>
#include "d2common_preset_env.h"
using namespace d2common_preset;

extern "C"
void d2common_116b0_port(uint32_t pLevel, const D2CommonPresetEnv* e) // 1.13c D2Common+0x116B0
{
	int32_t b = static_cast<int32_t>(((Lcg(pLevel) & 1) << 1) | 1);
	int32_t a = static_cast<int32_t>(Lcg(pLevel) & 3);
	switch (D(pLevel + 0x1D0))
	{
	case 0x2F:
	{
		uint32_t ebx = e->f10a00(pLevel);
		uint32_t ebp = e->f000(ebx, 1, 1);
		if (ebp)
		{
			e->pick(ebx, 1);
			ebx = ebp;
			e->pick(ebx, 1);
		}
		uint32_t edi = e->f000(ebp, 1, 1);
		if (edi)
		{
			e->pick(ebp, 1);
			e->pick(edi, 1);
		}
		ebp = e->f000(edi, 0, 0);
		if (ebp)
		{
			e->pick(edi, 1);
			Tag(ebp, 0x14D, 0);
		}
		ebx = e->f10820(pLevel);
		ebp = e->f000(ebx, 2, 1);
		if (ebp)
		{
			e->pick(ebx, 1);
			ebx = ebp;
			e->pick(ebx, 1);
		}
		edi = e->f000(ebp, 2, 1);
		if (edi)
		{
			e->pick(ebp, 1);
			e->pick(edi, 1);
		}
		ebp = e->f000(edi, static_cast<uint32_t>(b), 0);
		if (ebp)
		{
			e->pick(edi, 1);
			Tag(ebp, 0x150, 0);
		}
		const uint32_t pLast = e->f000(ebp, static_cast<uint32_t>(b), 0);
		if (pLast)
			e->pick(pLast, 1);
		Step(e, pLevel, 0x96788, &a);
		break;
	}
	case 0x30:
		Step(e, pLevel, 0x96748, &a);
		Step(e, pLevel, 0x967C8, &a);
		Step(e, pLevel, 0x96788, &a);
		break;
	case 0x31:
		Step(e, pLevel, 0x96748, &a);
		Step(e, pLevel, 0x96808, &a);
		break;
	case 0x41:
		Step(e, pLevel, 0x96748, &a);
		Step(e, pLevel, 0x96848, &a);
		break;
	default:
		break;
	}
}
