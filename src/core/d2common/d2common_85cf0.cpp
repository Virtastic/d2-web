// 1.13c D2Common+0x85CF0: [esp+4] = drlg (-> EAX for 0x85A80); ids {0x18C,0x18D}, flag 0. ret 4.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85cf0_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t ids[2] = {0x18C, 0x18D};
	f(drlg, ids, 2, 0);
}
