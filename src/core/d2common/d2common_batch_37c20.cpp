// 1.13c D2Common implementation batch: +0x37C20. Body follows the cached 1.13c decompile
// (build-wasm/ghidra-d2common/decomp/0x37C20.c). Retail pointers travel as 32-bit image addresses
// and the records are read by their retail byte offsets, as in the other d2common batch files.
// Callees that are still-UNMAPPED rows are env-seam function pointers; a missing seam contributes
// nothing. The fatal null-argument path (0x547) is modelled as a return.
#include <cstdint>
#include <cstring>
#include <emscripten.h>

namespace
{
	inline uint8_t* P(uint32_t a) { return reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(a)); }
	inline uint32_t R(const uint8_t* b, int32_t o) { uint32_t v; std::memcpy(&v, b + o, 4); return v; }
	inline uint32_t& W(uint8_t* b, int32_t o) { return *reinterpret_cast<uint32_t*>(b + o); }

	// Retail image data: gnTimeRates[0] and gNormalEnvironmentCycle[ENVCYCLE_NOON].nPeriodOfDay
	// (DAT_6fdeb238 / DAT_6fdeb17c). Both tables hold the same values in the vendored 1.10f twin.
	constexpr uint32_t kRetailTimeRate0 = 0x80;
	constexpr uint32_t kRetailNoonPeriodOfDay = 0;
}

// Env seams for the two callees this row makes; both are still-UNMAPPED rows.
struct D2CommonBatch37C20Env {
	int32_t (*sub2D2C0)(int32_t nLevelId);   // 0x2D2C0, answer discarded here
	void    (*sub376F0)(int32_t nLevelId);   // 0x376F0, light intensity update
};

// 1.13c D2Common+0x37C20: reset the environment record at wrapper+0x04 to noon. Its time-rate
// index (+0x2C) and eclipse flag (+0x30) clear, the time rate (+0x28) takes gnTimeRates[0], the
// cycle index (+0x00) becomes ENVCYCLE_NOON with nTicks (+0x08) zero and the period of day
// (+0x04) ENVPERIOD_DAY, then 0x2D2C0(0) and 0x376F0(0) run.
extern "C"
EMSCRIPTEN_KEEPALIVE void d2_fn_d2common_37c20(uint32_t pWrapper, const D2CommonBatch37C20Env* env)
{
	uint8_t* wrapper = P(pWrapper);
	if (!wrapper)
		return;                                     // retail fatal 0x547

	uint8_t* rec = P(R(wrapper, 0x04));
	if (!rec)
		return;

	W(rec, 0x2C) = 0;
	W(rec, 0x28) = kRetailTimeRate0;
	W(rec, 0x00) = 2;
	W(rec, 0x08) = 0;
	W(rec, 0x04) = kRetailNoonPeriodOfDay;

	if (env != nullptr && env->sub2D2C0 != nullptr)
		env->sub2D2C0(0);
	if (env != nullptr && env->sub376F0 != nullptr)
		env->sub376F0(0);

	W(rec, 0x30) = 0;
}
