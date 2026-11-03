// 1.13c D2Common+0x85B80: ESI = drlg (-> EAX for 0x85A80), plain ret. One frame array
// {0x188,0x189, 0x191,0x192,0x196,0x197,0x193}: the first call takes the last five (flag 0), the second the first two
// (flag 1) -- so the second table's pointer sees the first call's ids after its own two words, as retail's frame lays out.
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C85A80Fn)(uint32_t drlg, uint32_t* ids, int32_t n, int32_t flag);
extern "C" void d2c_85b80_port(uint32_t drlg, D2C85A80Fn f)
{
	uint32_t fr[7] = {0x188, 0x189, 0x191, 0x192, 0x196, 0x197, 0x193};
	f(drlg, fr + 2, 5, 0);
	f(drlg, fr, 2, 1);
}
