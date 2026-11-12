// 1.13c D2Client+0xA0730: one ignored stack word (ret 4). Always 1.
#include <cstdint>
extern "C" uint32_t d2c_a0730() { return 1; }
