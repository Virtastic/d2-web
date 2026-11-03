// 1.13c D2Common+0x68800 (ordinal none): re-resolve a node's entry record. One function per file (pb8 batch).
// Retail is thiscall: ESI = node, param_1 on the stack; ECX (this) only feeds the callee's upper bits and is not read.
// The callee 0x6C5D0 is __fastcall (ECX = entry type byte, EDX = wanted type); it has no ported body yet, so it is
// an env seam. Fatal paths (null node 0x273, null entry 0x274) go through the fatal seam: retail never returns there.
#include <cstdint>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }
}

extern "C"
EMSCRIPTEN_KEEPALIVE void D2Common_68800(uint32_t esi, int32_t param_1,
	uint32_t (*c6c5d0)(uint32_t ecx, uint32_t edx),
	void (*fatal)(int32_t))
{
	if (esi == 0)
	{
		if (fatal) fatal(0x273);
		return;
	}
	uint8_t* node = P(esi);
	uint32_t entry = W(node, 0x10);
	if (entry == 0)
	{
		if (fatal) fatal(0x274);
		return;
	}
	uint8_t cur = *(P(entry) + 0x2c);
	if (cur == 'b')
		return;
	// (param_1 != 0xb) - 1 is 0 or all ones; & 6 gives 6 or 0; the wanted type is 'l' (0x6c) or 'r' (0x72).
	uint8_t want = static_cast<uint8_t>(((static_cast<uint32_t>(param_1 != 0xb) - 1u) & 6u) + 0x6c);
	if (cur != want && c6c5d0)
		W(node, 0x10) = c6c5d0(cur, want);
}
