// 1.13c D2Common+0x85D90: [esp+4] = drlg (-> EAX for 0x85A80), ret 4. Seven ids {0x18B,0x19B,0x191,0x192,0x18F,0x18E,0x193}, flag 0.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85d90_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t fr[7] = {0x18B, 0x19B, 0x191, 0x192, 0x18F, 0x18E, 0x193};
	f(drlg, fr, 7, 0);
}
