// Storm batch b59-sto-a: UTF-16 string length.
#include <cstdint>

// 1.13c Storm+0x6868
int Storm_Sub6868(const int16_t* s) {
    const int16_t* p = s;
    while (*p++ != 0) {}
    return (int)(p - s) - 1;
}
