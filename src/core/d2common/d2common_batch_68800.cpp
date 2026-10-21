// D2Common batch 68800. Env-seam style (see d2common_batch_44a30.cpp): callees are function
// pointers, retail register args explicit. Implementation only; unvectored.
#include <cstdint>
#include <emscripten.h>

// 1.13c D2Common+0x68800: ESI = node (null -> fatal 0x273); node+0x10 = entry pointer (null ->
// fatal 0x274). Entry byte +0x2c: unless it is 'b', re-resolve it to entry 0x6c (0x72 when a1 == 0xb)
// via sub_6C5D0 when it differs, and store the result at node+0x10. fatal never returns in retail.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_68800(uintptr_t esi, int32_t a1, uintptr_t (*c6c5d0)(int32_t, int32_t),
	void (*fatal)(int32_t))
{
	if (esi == 0) { fatal(0x273); return; }
	uint32_t e = *(uint32_t*)(esi + 0x10);
	if (e == 0) { fatal(0x274); return; }
	char c1 = *(char*)(uintptr_t)(e + 0x2c);
	if (c1 == 'b') return;
	char c2 = (char)(((a1 != 0xb) - 1u & 6) + 0x6c);
	if (c1 != c2) *(uint32_t*)(esi + 0x10) = (uint32_t)c6c5d0((int32_t)(uint8_t)c1, c2);
}
