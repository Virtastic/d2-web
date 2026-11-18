// 1.13c D2Lang implementation batch: +0x08FE0.
// The ELF-style string hash used to index .tbl string tables: fold each signed byte of the
// NUL-terminated narrow key into a 28-bit accumulator, then reduce modulo the table size.
#include <cstdint>

// 1.13c D2Lang+0x08FE0 (#10000): hash a NUL-terminated string modulo nMod.
// Retail takes the byte sign-extended (its char is signed); the fold keeps the accumulator at 28
// bits. nMod is the table's hash-table size and is never 0 for a loaded table -- a 0 divides by
// zero exactly as retail's idiv does. TODO(spec): caller contract for nMod = 0 (0x9360 caller).
extern "C" uint32_t d2lang_8fe0(const char* pszString, uint32_t nMod)
{
	uint32_t nHash = 0;
	while (*pszString != '\0')
	{
		nHash = nHash * 0x10 + static_cast<uint32_t>(static_cast<int32_t>(*pszString));
		if ((nHash & 0xf0000000u) != 0)
			nHash = ((nHash & 0xf0000000u) >> 0x18 ^ nHash) & 0x0fffffffu;
		++pszString;
	}
	return nHash % nMod;
}
