// 1.13c D2Common+0x54200: teardown of the level/monster table group, cdecl(void). Retail-layout (32-bit) reader over the static table
// block at RVA 0xA0880 (`tb`, offsets below are relative to it) and the allocation-header flag at RVA 0x99E20 (`flag`).
// A loaded slot is released through the Fog free thunk 0x9228 (ECX = 0, EDX = the pointer, backed off by 4 when the header flag is set;
// stack: the DataTbls.cpp name, line 0x8F6 with the flag else 0x8F8, 0); some slots also release their linker through 0x9246.
// Order: +0x238 (count +0x23C := 0), +0x00, the siblings 0x53EB0 0x53AD0 0x53F10 0x53F70, +0xD0 (linker +0xD4; slot := 0 either way),
// +0xE8 (linker +0xEC), +0x288 (linker +0x284; +0x28C := 0 either way), +0xDC (linker +0xE0, +0xE4 := 0), the siblings 0x539E0 0x534B0,
// +0x1C8 (+0x1CC := 0), then the tail jump to 0x54090. The callees are seams.
#include <cstdint>
#include <cstring>

struct D2Common54200Env { void (*freeMem)(uint32_t p, uint32_t line); void (*freeLinker)(uint32_t linker); void (*sub)(uint32_t rva); };

namespace {
inline uint32_t Rd32(uint32_t a) { uint32_t v; memcpy(&v, reinterpret_cast<const void*>(static_cast<uintptr_t>(a)), 4); return v; }
inline void Wr32(uint32_t a, uint32_t v) { memcpy(reinterpret_cast<void*>(static_cast<uintptr_t>(a)), &v, 4); }
// One guarded free: false when the slot is empty.
inline bool Release(uint32_t slot, uint32_t flag, const D2Common54200Env* env)
{
	const uint32_t p = Rd32(slot);
	if (p == 0) return false;
	if (flag != 0) env->freeMem(p - 4, 0x8F6);
	else env->freeMem(p, 0x8F8);
	return true;
}
}

extern "C" void D2Common_54200_Raw(uint32_t tb, uint32_t flag, const D2Common54200Env* env)
{
	if (Release(tb + 0x238, flag, env)) Wr32(tb + 0x23C, 0);
	Release(tb + 0x00, flag, env);
	env->sub(0x53EB0);
	env->sub(0x53AD0);
	env->sub(0x53F10);
	env->sub(0x53F70);
	if (Release(tb + 0xD0, flag, env)) env->freeLinker(Rd32(tb + 0xD4));
	// retail loads the next slot before it clears this one; the two do not overlap
	Wr32(tb + 0xD0, 0);
	if (Release(tb + 0xE8, flag, env)) env->freeLinker(Rd32(tb + 0xEC));
	if (Release(tb + 0x288, flag, env)) env->freeLinker(Rd32(tb + 0x284));
	const uint32_t slotDC = Rd32(tb + 0xDC);
	Wr32(tb + 0x28C, 0);
	if (slotDC != 0) {
		if (flag != 0) env->freeMem(slotDC - 4, 0x8F6);
		else env->freeMem(slotDC, 0x8F8);
		env->freeLinker(Rd32(tb + 0xE0));
		Wr32(tb + 0xE4, 0);
	}
	env->sub(0x539E0);
	env->sub(0x534B0);
	if (Release(tb + 0x1C8, flag, env)) Wr32(tb + 0x1CC, 0);
	env->sub(0x54090);
}
