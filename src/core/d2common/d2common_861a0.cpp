// 1.13c D2Common+0x861A0 (with its tail target 0x86040): EAX = drlg (kept in ESI), plain ret. Runs 0x176F0 on the drlg (and 0x17180
// too unless the level type at drlg+0x1D0 is 0x6C); type 0x6C then runs 0x85FD0 (EDI = drlg); types 0x68..0x6A run 0x86000, three
// 0x43100 calls (EAX = drlg, 1/2/3, 0x31F), then 0x86040: 0x42CE0 (ESI = drlg, x, -1, 0, 0xF) calls over two .rdata tile-base
// words (0x9560C / 0x95618, indexed by type) as listed below. Steps with a register/stack convention of their own go through
// call(rva, drlg); 0x43100 and 0x42CE0 are passed with their true arguments.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C861A0Call)(uint32_t rva, uint32_t drlg);
typedef void (*D2C43100Fn)(uint32_t drlg, int32_t a, int32_t b);
typedef void (*D2C42CE0Fn)(uint32_t drlg, int32_t x, int32_t y, int32_t z, int32_t w);
static const int32_t kTbl9560C[3] = { 0x33C, 0x340, 0x340 };   // indexed by type - 0x68 (the third-group base)
static const int32_t kTbl95618[3] = { 0x32C, 0x331, 0x337 };   // indexed by type - 0x68 (the first-group base)
extern "C" void d2c_861a0_port(uint32_t drlg, D2C861A0Call call, D2C43100Fn f43100, D2C42CE0Fn f42ce0)
{
	const uint32_t* t = reinterpret_cast<const uint32_t*>(uintptr_t(drlg) + 0x1D0);
	if (*t == 0x6C) {
		call(0x176F0, drlg);
	} else {
		call(0x176F0, drlg);
		call(0x17180, drlg);
	}
	const int32_t ty = int32_t(*t);
	if (ty < 0x68) return;
	if (ty > 0x6A) {
		if (ty == 0x6C) call(0x85FD0, drlg);
		return;
	}
	call(0x86000, drlg);
	f43100(drlg, 1, 0x31F);
	f43100(drlg, 2, 0x31F);
	f43100(drlg, 3, 0x31F);
	// 0x86040
	if (*t == 0x6A) f42ce0(drlg, 0x32B, -1, 0, 0xF);
	const int32_t b2 = kTbl95618[*t - 0x68], b1 = kTbl9560C[*t - 0x68];
	f42ce0(drlg, b2, -1, 0, 0xF);
	for (int k = 1; k <= 3; k++) { f42ce0(drlg, b2 + k, -1, 0, 0xF); f42ce0(drlg, b2 + k, -1, 0, 0xF); }
	if (*t == 0x69) f42ce0(drlg, 0x336, -1, 0, 0xF);
	for (int k = 0; k < 4; k++) f42ce0(drlg, b2 + 4, -1, 0, 0xF);
	f42ce0(drlg, b1, -1, 0, 0xF);
	for (int k = 1; k <= 2; k++) { f42ce0(drlg, b1 + k, -1, 0, 0xF); f42ce0(drlg, b1 + k, -1, 0, 0xF); }
	for (int k = 0; k < 4; k++) f42ce0(drlg, b1 + 3, -1, 0, 0xF);
}
