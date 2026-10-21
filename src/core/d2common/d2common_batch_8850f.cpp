// D2Common 1.13c batch: CRT 80-bit extended multiply (pA *= pB), mantissa product in a 12-byte accumulator.
#include <cstdint>
#include <cstring>
#include <emscripten.h>
void d2common_shl_12_87cf0(uint32_t* words);   // d2common_batch_9330.cpp (C++ linkage)
void d2common_shr_12_87d1e(uint32_t* words);   // d2common_batch_9330.cpp (C++ linkage)


extern "C" {
uint32_t D2Common_87C71_Addl(uint32_t nLeft, uint32_t nRight, uint32_t* pOut);   // d2common_batch_tiny_1018b.cpp

static inline uint16_t rd16(const uint8_t* p) { uint16_t v; memcpy(&v, p, 2); return v; }
static inline uint32_t rd32(const uint8_t* p) { uint32_t v; memcpy(&v, p, 4); return v; }
static inline void wr16(uint8_t* p, uint16_t v) { memcpy(p, &v, 2); }
static inline void wr32(uint8_t* p, uint32_t v) { memcpy(p, &v, 4); }

// 1.13c D2Common+0x8850F
EMSCRIPTEN_KEEPALIVE void d2common_mul_ext_8850f(uint8_t* pA, const uint8_t* pB)
{
	alignas(4) uint8_t acc[12] = {0};   // local_14.. : bytes 2..11 are the 80-bit result mantissa
	int32_t sticky = 0;
	const uint16_t expB = rd16(pB + 10) & 0x7fff;
	const uint16_t expA = rd16(pA + 10) & 0x7fff;
	const uint16_t sign = (rd16(pB + 10) ^ rd16(pA + 10)) & 0x8000;
	uint16_t sum = (uint16_t)(expB + expA);
	uint16_t e;
	bool store = false;

	if (!(expA < 0x7fff && expB < 0x7fff && sum < 0xbffe)) goto overflow;
	if (sum < 0x3fc0) goto zero;
	if (expA == 0) {
		sum++;
		e = 0;
		if (!((rd32(pA + 8) & 0x7fffffffu) != 0 || rd32(pA + 4) != 0 || rd32(pA) != 0)) { store = true; goto done; }
	}
	if (expB == 0) {
		sum++;
		if ((rd32(pB + 8) & 0x7fffffffu) == 0 && rd32(pB + 4) == 0 && rd32(pB) == 0) goto zero;
	}
	{
		int32_t off = 0;           // local_30
		uint8_t* acc_p = acc + 4;  // local_20
		for (int32_t n = 5; n > 0; --n, acc_p += 2, ++off) {
			if (n > 0) {
				const uint8_t* a = pA + off * 2;
				const uint8_t* b = pB + 8;
				for (int32_t k = n; k != 0; --k, a += 2, b -= 2) {
					uint32_t cur = rd32(acc_p - 4);
					uint32_t r = D2Common_87C71_Addl(cur, (uint32_t)rd16(b) * (uint32_t)rd16(a), &cur);
					wr32(acc_p - 4, cur);
					if (r != 0) wr16(acc_p, (uint16_t)(rd16(acc_p) + 1));
				}
			}
		}
	}
	e = (uint16_t)(sum + 0xc002);
	if ((int16_t)e >= 1) {
		do {
			if (acc[11] & 0x80) break;
			d2common_shl_12_87cf0(reinterpret_cast<uint32_t*>(acc));
			e--;
		} while ((int16_t)e > 0);
	}
	if ((int16_t)e < 1) {
		e--;
		if ((int16_t)e < 0) {
			uint32_t cnt = (uint16_t)(-(int16_t)e);
			e = 0;
			do {
				if (acc[0] & 1) sticky++;
				d2common_shr_12_87d1e(reinterpret_cast<uint32_t*>(acc));
			} while (--cnt != 0);
			if (sticky != 0) acc[0] |= 1;
		}
	}
	if (rd16(acc) > 0x8000 || (rd32(acc) & 0x1ffff) == 0x18000) {
		uint32_t lo = rd32(acc + 2), mid = rd32(acc + 6);
		uint16_t hi = rd16(acc + 10);
		if (lo == 0xffffffffu) {
			lo = 0;
			if (mid == 0xffffffffu) {
				mid = 0;
				if (hi == 0xffff) { e++; hi = 0x8000; } else hi++;
			} else mid++;
		} else lo++;
		wr32(acc + 2, lo); wr32(acc + 6, mid); wr16(acc + 10, hi);
	}
	if (e > 0x7ffe) goto overflow;
	memcpy(pA, acc + 2, 10);
	e |= sign;
	store = true;
done:
	if (store) wr16(pA + 10, e);
	return;
zero:
	wr32(pA + 8, 0); wr32(pA + 4, 0); wr32(pA, 0);
	return;
overflow:
	wr32(pA + 4, 0); wr32(pA, 0);
	wr32(pA + 8, (sign != 0 ? 0x80000000u : 0u) + 0x7fff8000u);
}
}
