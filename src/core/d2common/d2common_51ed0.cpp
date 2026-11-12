// 1.13c D2Common+0x51ED0 over retail 32-bit layouts: drop every transform state a unit carries (stdcall unit, ret 4).
// The engine form is D2Common_sub_51ED0 (d2common_batch_2fe70.cpp) on the host structs; this form takes the addresses of the
// data tables (sgptDataTables), the bit-mask table (gdwBitMasks) and the unit as retail lays them out, and reaches the five
// callees through seams: 0x37F80 (ECX list-ex, EDX state), 0x3A3B0 (stdcall list), 0x3A9F0 (ECX list),
// 0x38730 (EBX state, stdcall unit, 0) and 0x665C0 (stdcall unit).
#include <cstdint>
#include <cstring>

struct D2Common51ED0Env { uint32_t (*getStateList)(uint32_t, int32_t); void (*expire)(uint32_t); void (*freeList)(uint32_t);
                          void (*toggleOff)(int32_t, uint32_t); void (*refresh)(uint32_t); };

namespace {
inline uint32_t rd32(uint32_t a) { uint32_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 4); return v; }
inline int16_t rd16(uint32_t a) { int16_t v; std::memcpy(&v, (const void*)(uintptr_t)a, 2); return v; }
}

// 1.13c D2Common+0x51ED0
uint32_t D2Common_51ED0_Raw(uint32_t dt, uint32_t mk, uint32_t unit, const D2Common51ED0Env* env)
{
	if (!unit || ((rd32(unit + 0xC8) >> 3) & 1) == 0) return 0;
	// the transform-state list: shorts at [dt+0x17C], count a signed short at dt+0x180, re-read each pass
	for (int32_t i = 0; i < rd16(dt + 0x180); ++i)
	{
		const int32_t nState = rd16(rd32(dt + 0x17C) + 2u * (uint32_t)i);
		const int32_t nType = (int32_t)rd32(unit);
		if (nType < 0 || (nType > 1 && nType != 3)) continue;
		if (nState < 0 || nState >= (int32_t)rd32(dt + 0xC4)) continue;
		const uint32_t ex = rd32(unit + 0x5C);
		if (!ex || (int32_t)rd32(ex + 0x10) >= 0) continue;
		if ((rd32(rd32(ex + 0x58) + 4u * (uint32_t)(nState >> 5)) & rd32(mk + 4u * (uint32_t)(nState & 31))) == 0) continue;
		if (const uint32_t sl = env->getStateList(ex, nState))
		{
			env->expire(sl);
			if ((int32_t)rd32(sl + 0x10) >= 0) env->freeList(sl);
		}
		env->toggleOff(nState, unit);
		env->refresh(unit);
	}
	return 1;
}
