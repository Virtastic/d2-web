// Fog batch b56a: __chkstk and __alldvrm (Fog.dll compiler helpers). The other ten rows of the batch
// take register args, call Win32 directly or use SEH, so they are left UNMAPPED.
#include <cstdint>
#include <initializer_list>

// 1.13c Fog+0x08310
extern "C" void fog_chkstk(uint32_t size, uint8_t* esp, uint32_t retaddr)
{
	if (size < 0x1000) {
		*(uint32_t*)(esp - size) = retaddr;
		return;
	}
	uint8_t* p = esp + 4;
	do {
		p -= 0x1000;
		size -= 0x1000;
	} while (size > 0xFFF);
	*(uint32_t*)(p - 4 - size) = retaddr;
}

// 1.13c Fog+0x21620
// Signed 64-bit divide: returns quotient in the low word, remainder in the high word.
extern "C" uint64_t fog_alldvrm(uint32_t lo, uint32_t hi, uint32_t dlo, uint32_t dhi)
{
	uint32_t neg = (int32_t)hi < 0;
	if (neg) {
		bool nz = lo != 0;
		lo = -lo;
		hi = -(uint32_t)nz - hi;
	}
	if ((int32_t)dhi < 0) {
		neg++;
		bool nz = dlo != 0;
		dlo = -dlo;
		dhi = -(uint32_t)nz - dhi;
	}
	uint32_t q, r;
	if (dhi == 0) {
		r = hi / dlo;
		q = (uint32_t)((((uint64_t)hi % dlo) << 32 | lo) / dlo);
	} else {
		uint32_t a = lo, b = dlo, c = hi, d = dhi;
		do {
			b = b >> 1 | (d & 1) << 31;
			a = a >> 1 | (c & 1) << 31;
			c >>= 1;
			d >>= 1;
		} while (d != 0);
		uint64_t t = (((uint64_t)c << 32) | a) / b;
		q = (uint32_t)t;
		uint64_t m = (uint64_t)dlo * (uint32_t)t;
		uint32_t mh = (uint32_t)(m >> 32);
		uint32_t s = mh + q * dhi;
		if (s < mh || hi < s || (hi <= s && lo < (uint32_t)m))
			q--;
		r = 0;
	}
	if (neg == 1) {
		bool nz = q != 0;
		q = -q;
		r = -(uint32_t)nz - r;
	}
	return (uint64_t)r << 32 | q;
}
