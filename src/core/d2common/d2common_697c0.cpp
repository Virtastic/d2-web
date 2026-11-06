// 1.13c D2Common+0x697C0 (sub_697C0): room-tile dispatch. Retail layout: EBX, ESI, EDI are inputs it forwards unchanged; stack = a0, a1 (room), a2 (type), ret 0xC.
// Asks 0x68780 (EAX = EBX; stack a1, a2==0, edi, esi, &out) for a node, with the out value cleared first. A node goes to 0x695D0
// (EAX = a2, ECX = node; stack a0, a1, out, ebx, edi, esi); none goes to 0x69470 (EAX = a2, ECX = a1; stack a0, ebx, edi, esi).
#include <cstdint>

struct D2Common697C0Env
{
	uintptr_t (*f68780)(uint32_t eaxEbx, uintptr_t a1, uint32_t isNull, uint32_t edi, uint32_t esi, uint32_t* out); // ret 0x14
	void (*f695d0)(uint32_t eaxA2, uintptr_t ecxNode, uint32_t a0, uintptr_t a1, uint32_t out, uint32_t ebx, uint32_t edi, uint32_t esi); // ret 0x18
	void (*f69470)(uint32_t eaxType, uintptr_t ecxA1, uint32_t a0, uint32_t ebx, uint32_t edi, uint32_t esi);        // ret 0x10
};

extern "C" void D2Common_697C0_Raw(uint32_t ebx, uint32_t esi, uint32_t edi, uint32_t a0, uintptr_t a1, uint32_t a2, const D2Common697C0Env* env)
{
	uint32_t out = 0;
	const uintptr_t node = env->f68780(ebx, a1, a2 == 0, edi, esi, &out);
	if (node != 0) {
		env->f695d0(a2, node, a0, a1, out, ebx, edi, esi);
		return;
	}
	env->f69470(a2, a1, a0, ebx, edi, esi);
}
