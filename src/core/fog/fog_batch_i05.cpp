// Fog implementation batch (i05fog): CRT 80-bit helpers, thin Safesock wrappers, per-thread purecall.
#include <cstdint>

// ---- 12-byte (96-bit) mantissa helpers; same semantics as the CRT ___shl_12/___add_12/___shr_12 ----
static void i05_shl12(uint32_t* p) {
	uint32_t a = p[0], b = p[1];
	p[0] = a << 1; p[1] = (b << 1) | (a >> 31); p[2] = (p[2] << 1) | (b >> 31);
}
static void i05_shr12(uint32_t* p) {
	p[0] = (p[0] >> 1) | (p[1] << 31); p[1] = (p[1] >> 1) | (p[2] << 31); p[2] >>= 1;
}
static void i05_add12(uint32_t* d, const uint32_t* s) {
	uint64_t t = (uint64_t)d[0] + s[0]; d[0] = (uint32_t)t;
	t = (uint64_t)d[1] + s[1] + (t >> 32); d[1] = (uint32_t)t;
	d[2] = d[2] + s[2] + (uint32_t)(t >> 32);
}

// 1.13c FOG+0x0C1D6 (__mantissa from decimal digits): pDigits[0..n) -> 96-bit mantissa * 10 each step,
// normalised so bit 79 is set; exponent word (bytes 10..11) starts at 0x404E and drops per shift.
extern "C" void fog_crt_mantissa_0C1D6(const char* pDigits, int32_t n, uint32_t* out) {  // 1.13c FOG+0x0C1D6
	int16_t e = 0x404E;
	out[0] = out[1] = out[2] = 0;
	for (; n; --n) {
		uint32_t t[3] = {out[0], out[1], out[2]};
		i05_shl12(out); i05_shl12(out);
		i05_add12(out, t);
		i05_shl12(out);
		uint32_t d[3] = {(uint32_t)(int32_t)*pDigits++, 0, 0};
		i05_add12(out, d);
	}
	if (out[2] == 0) {
		uint32_t hi;
		do {
			e -= 0x10;
			hi = out[1] >> 16;
			out[1] = (out[0] >> 16) | (out[1] << 16);
			out[0] <<= 16;
		} while (hi == 0);
		out[2] = hi;
	}
	while (!(out[2] & 0x8000)) { i05_shl12(out); --e; }
	*(int16_t*)((char*)out + 10) = e;
}

// 1.13c FOG+0x22C1A: decrement the calling thread's _purecall nesting count if positive.
static thread_local int32_t g_fog_purecall;
extern "C" void fog_sub_22c1a() {  // 1.13c FOG+0x22C1A
	if (g_fog_purecall > 0) --g_fog_purecall;
}

// 1.13c FOG+0x16DD0 (ordinal 10003) and FOG+0x1EC90: thin callers of Safesock connect (0x16B10).
// The connect body lives in fn/fog_batch_16900.cpp behind FogSockApi; the OS-bound hook is injected here.
typedef void* (*fog_connect_fn)(uintptr_t host, uint32_t port);
extern "C" fog_connect_fn g_fog_safesock_connect_16b10;
fog_connect_fn g_fog_safesock_connect_16b10 = nullptr;
extern "C" void* fog_sub_16dd0(uintptr_t host, uint32_t port) {  // 1.13c FOG+0x16DD0
	return g_fog_safesock_connect_16b10 ? g_fog_safesock_connect_16b10(host, port) : nullptr;
}
extern "C" uint32_t fog_safesock_connect_1ec90(uint8_t* p) {  // 1.13c FOG+0x1EC90
	void* s = fog_sub_16dd0((uintptr_t)(p + 8), *(uint32_t*)(p + 0x10c));
	*(void**)(p + 0x110) = s;
	return 1;
}
