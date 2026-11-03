// 1.13c D2Common+0x85C50: [esp+4] = drlg, which the function loads into EAX for 0x85A80;
// hands the four ids {0x19D,0x198,0x199,0x19A} to 0x85A80 (flag 0). The table lives in this function's frame, so the
// callee seam receives a stack pointer. ret 4.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85c50_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t ids[4] = {0x19D, 0x198, 0x199, 0x19A};
	f(drlg, ids, 4, 0);
}
