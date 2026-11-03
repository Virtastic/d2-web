// 1.13c D2Common+0x85D20: ESI = drlg. Frame array of eight ids: call 1 the first four (flag 0), call 2 the last four (flag 1).
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85d20_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t fr[8] = {0x18B, 0x19B, 0x190, 0x18E, 0x194, 0x195, 0x196, 0x197};
	f(drlg, fr, 4, 0);
	f(drlg, fr + 4, 4, 1);
}
