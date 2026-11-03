// 1.13c D2Common+0x85BE0: ESI = drlg (-> EAX for 0x85A80), plain ret. Frame array
// {0x19B, 0x18B,400,0x18E,0x194,0x195}: first call the last five (flag 0), then the first word twice (flag 1).
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85be0_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t fr[6] = {0x19B, 0x18B, 400, 0x18E, 0x194, 0x195};
	f(drlg, fr + 1, 5, 0);
	f(drlg, fr, 1, 1);
	f(drlg, fr, 1, 1);
}
