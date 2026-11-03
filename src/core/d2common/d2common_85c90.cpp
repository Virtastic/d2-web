// 1.13c D2Common+0x85C90: ESI = drlg (-> EAX for 0x85A80). One frame array {0x18B,0x19B,0x18F,0x18E,0x193}: call 1 takes fr+1 (4 ids, flag 0), calls 2 and 3 the first word (flag 1).
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85c90_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t fr[5] = {0x18B, 0x19B, 0x18F, 0x18E, 0x193};
	f(drlg, fr + 1, 4, 0);
	f(drlg, fr, 1, 1);
	f(drlg, fr, 1, 1);
}
