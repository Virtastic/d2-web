// 1.13c D2Common+0x85DF0: ESI = drlg (copied to EAX for 0x43100, 2 stack words, ret 8); plain ret.
// 0x43100(drlg, 2, 0x16C), (drlg, 1, 0x16C), (drlg, 3, 0x16C).
#include <cstdint>
#include <emscripten.h>
typedef void (*D2C43100Fn)(uint32_t drlg, int32_t a, int32_t b);
extern "C" void d2c_85df0_port(uint32_t drlg, D2C43100Fn f)
{
	f(drlg, 2, 0x16C);
	f(drlg, 1, 0x16C);
	f(drlg, 3, 0x16C);
}
