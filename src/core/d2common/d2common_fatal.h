// Retail fatal exit for D2Common ports: Ordinal_10024(&DAT_6fdda728, Ordinal_10265(code)); _exit(-1).
// The Fog ordinals are weak seams, so a link without them traps instead of failing to resolve.
#pragma once
#include <cstdint>

extern "C" {
__attribute__((weak)) uint32_t Ordinal_10265(uint32_t);
__attribute__((weak)) void Ordinal_10024(const void*, uint32_t);
__attribute__((weak)) char DAT_6fdda728[1];
}

[[noreturn]] inline void d2common_fatal(uint32_t nCode)
{
	if (Ordinal_10265 && Ordinal_10024)
		Ordinal_10024(DAT_6fdda728, Ordinal_10265(nCode));
	__builtin_trap();
}
