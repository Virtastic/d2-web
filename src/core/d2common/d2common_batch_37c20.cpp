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

// Env seams (defined in d2common_batch_37ca0.cpp, null until the environment installs them).
extern "C"
{
	extern uint32_t (*g_d2common_fn_2D2C0)(int32_t);                              // 0x2D2C0
	extern void (*g_d2common_fn_376F0)(uint32_t ecx, uint32_t edi, void* rec);    // 0x376F0
}

// 1.13c D2Common+0x37C20: reset the environment record at wrapper+0x04 to noon. Its time-rate
// index (+0x2C) and eclipse flag (+0x30) clear, the time rate (+0x28) takes gnTimeRates[0], the
// cycle index (+0x00) becomes ENVCYCLE_NOON with nTicks (+0x08) zero and the period of day
// (+0x04) ENVPERIOD_DAY, then 0x2D2C0(0) runs (its byte goes to EDI) and 0x376F0 (ECX 0, ESI rec).
extern "C"
void d2common_37c20_port(uint32_t pWrapper)
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

	const uint32_t nBand = g_d2common_fn_2D2C0 ? g_d2common_fn_2D2C0(0) & 0xFF : 0;
	if (g_d2common_fn_376F0)
		g_d2common_fn_376F0(0, nBand, rec);

	W(rec, 0x30) = 0;
}
