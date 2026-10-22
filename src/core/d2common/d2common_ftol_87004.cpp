// 1.13c D2Common+0x87004: CRT float-to-int64 helper. The operand arrives in x87 ST0 (modelled as a
// double). Retail rounds to nearest, then steps the result one toward zero whenever the rounding went
// away from zero, so the net effect is truncation toward zero; a value that rounds to 0 returns 0.
#include <cstdint>
#include <emscripten.h>

extern "C"
EMSCRIPTEN_KEEPALIVE uint64_t d2_fn_d2common_87004(double fValue)
{
	return static_cast<uint64_t>(static_cast<int64_t>(fValue));
}
