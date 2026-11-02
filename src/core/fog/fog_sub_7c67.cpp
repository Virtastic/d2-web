// 1.13c Fog+0x7C67: retail body is an empty SEH frame and returns 1.
#include <cstdint>

extern "C" uint32_t fog_sub_7c67(void) { return 1; }
