// 1.13c D2Win+0xF860: a type-6 control is ready when words 0x11 and 0x12 are both non-zero.
#include <cstdint>
extern "C" int d2win_Ctrl6Ready(const uint32_t* c) {
    if (c[0] != 6) return -1;   // retail: error text via ordinal 10024, then _exit(-1)
    return c[0x11] && c[0x12];
}
