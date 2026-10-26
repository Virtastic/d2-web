// D2Common 1.13c batch: CRT digit string -> 80-bit extended mantissa (___mtold12 style).
#include <cstdint>
#include <emscripten.h>
void d2common_shl_12_87cf0(uint32_t* words);   // d2common_batch_9330.cpp
extern "C" void D2Common_87C92_Add12(uint32_t* pA, const uint32_t* pB);

extern "C" {
// 1.13c D2Common+0x87D4B
EMSCRIPTEN_KEEPALIVE void D2Common_87D4B(const char* digits, int32_t count, uint32_t* out)
{
	int16_t exp = 0x404e;
	out[0] = out[1] = out[2] = 0;
	for (; count != 0; --count) {
		uint32_t t[3] = { out[0], out[1], out[2] };
		d2common_shl_12_87cf0(out);
		d2common_shl_12_87cf0(out);
		D2Common_87C92_Add12(out, t);   // out = out*5
		d2common_shl_12_87cf0(out);     // out = out*10
		t[0] = (uint32_t)(int32_t)(int8_t)*digits++; t[1] = 0; t[2] = 0;
		D2Common_87C92_Add12(out, t);
	}
	if (out[2] == 0) {
		uint32_t hi;
		do {
			exp = (int16_t)(exp - 0x10);
			hi = out[1] >> 16;
			out[1] = (out[0] >> 16) | (out[1] << 16);
			out[0] <<= 16;
		} while (hi == 0);
		out[2] = hi;
	}
	while (!(out[2] & 0x8000)) {
		d2common_shl_12_87cf0(out);
		exp = (int16_t)(exp - 1);
	}
	*(int16_t*)((uint8_t*)out + 10) = exp;
}
}
