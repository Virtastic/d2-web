// 1.13c D2Common+0x86220 (CRT x87 extended-precision classifier helper). Args: first long double as
// mantissa lo (p1), mantissa hi (p2), sign/exponent word (p3); p4 is padding; second operand's
// mantissa hi (p5) and sign/exponent (p6). The only x87 op is a multiply by the float constant
// 2^64 (retail bytes 0x5F000000 at 0x6FDEF148); on an unnormalised denormal it is exact, so it is
// modelled as normalise-and-rebias (e = 1 + 64 - clz, mantissa << clz), sign kept. Table at
// 0x6FDEF130: 16 flag bytes, nonzero at indices 1,4,7,10,13.
#include <cstdint>

// 1.13c D2Common+0x86220
uint32_t d2common_sub_86220(int32_t p1, uint32_t p2, uint16_t p3, uint32_t p4, uint32_t p5, uint32_t p6)
{
	static const uint8_t kFlags[16] = { 0, 1, 0, 0, 4, 0, 0, 7, 0, 0, 10, 0, 0, 13, 0, 0 };
	(void)p4;
	uint32_t lo = static_cast<uint32_t>(p1);
	while (!(p2 >> 31)) {
		if (lo == 0 && p2 == 0) return 0;
		if (p3 & 0x7fff) return p3 & 0x7fff;
		uint32_t r = p6 & 0x7fff;
		if (r == 0) {
			if (p5 >> 31) return p5 * 2;
		} else if (r == 0x7fff || (r = p5 * 2, !(p5 >> 31))) {
			return r;
		}
		// x87 fmul by 2^64: exponent field is 0 here, so the value is a denormal (effective exp 1).
		uint64_t m = (static_cast<uint64_t>(p2) << 32) | lo;
		uint32_t s = static_cast<uint32_t>(__builtin_clzll(m)); // m != 0 (checked above)
		m <<= s;
		p3 = static_cast<uint16_t>((p3 & 0x8000) | (1 + 64 - s));
		lo = static_cast<uint32_t>(m);
		p2 = static_cast<uint32_t>(m >> 32);
	}
	uint32_t u = (p2 * 2) ^ 0x0e000000;
	if (u & 0x0e000000) return u;
	u = (p2 & 0x7fffffff) >> 27;
	if (kFlags[u]) {
		if ((p3 & 0x7fff) != 0 && (p3 & 0x7fff) != 0x7fff) {
			if ((p6 & 0x7fff) != 1) return p6 & 0x7fff;
			return 1;
		}
		return p3 & 0x7fff;
	}
	return u;
}
