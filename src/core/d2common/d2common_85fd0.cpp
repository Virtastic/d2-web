// 1.13c D2Common+0x85FD0: EDI = drlg (ECX for 0x42A70), plain ret. For i in 0..24 calls
// 0x42A70(ecx = drlg, edx = (i%5)*3, y = (i/5)*3, tbl[i], -1, 0) with the 25-dword .rdata table at 0x95748 (0x344 except
// the six ids 0x35D,0x35A,0x35E,0x35B,0x35C,0x359). 0x42A70: ECX, EDX, then four stack words, ret 0x10.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C42A70Fn)(uint32_t ecx, int32_t edx, int32_t y, int32_t tbl, int32_t n1, int32_t z);
static const uint32_t kTbl95748[25] = {
	0x344, 0x344, 0x344, 0x344, 0x344, 0x344, 0x344, 0x35D, 0x344, 0x344, 0x344, 0x35A, 0x35E,
	0x35B, 0x344, 0x344, 0x344, 0x35C, 0x344, 0x344, 0x344, 0x344, 0x359, 0x344, 0x344};
extern "C" void d2c_85fd0_port(uint32_t drlg, D2C42A70Fn f)
{
	for (uint32_t i = 0; i < 25; i++)
		f(drlg, int32_t((i % 5) * 3), int32_t((i / 5) * 3), int32_t(kTbl95748[i]), -1, 0);
}
