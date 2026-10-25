// 1.13c D2Lang implementation batch b51. Not ported (no faithful portable form: SEH/TLS/lock/stack-cookie
// internals or unported callees): 0x260C 0x272E 0x1C25 0x25FE 0x152C 0x22CA 0x2478 0x24B3 0x1D6C 0x1B55.
#include <cstdint>

// 1.13c D2Lang+0x026E0 ___ascii_stricmp: ASCII case-fold compare, returns -1/0/1.
extern "C" int d2lang_26e0(const char* a, const char* b)
{
	for (;;)
	{
		uint8_t x = static_cast<uint8_t>(*a++), y = static_cast<uint8_t>(*b++);
		if (x != y)
		{
			uint8_t fx = (x >= 'A' && x <= 'Z') ? x + 0x20 : x;
			uint8_t fy = (y >= 'A' && y <= 'Z') ? y + 0x20 : y;
			if (fx != fy)
				return fx < fy ? -1 : 1;
		}
		if (y == 0)
			return 0;
	}
}

// 1.13c D2Lang+0x01B90 __aulldvrm: unsigned 64-bit divide; quotient returned, remainder via *rem.
extern "C" uint64_t d2_fn_d2lang_1b90(uint64_t n, uint64_t d, uint64_t* rem)
{
	uint64_t q = n / d;
	if (rem)
		*rem = n % d;
	return q;
}
