// 1.13c D2CMP+0x32EC (write_multi_char, VS2003 CRT; same bytes as D2Common+0x392A): write_char n times,
// stopping early once *count is -1.
#include "core/d2common/output_3985.h"
extern "C" void d2cmp_32b9(uint8_t b, D2CommonStringStream2814* s, const D2CommonOutputCalls3985* calls,
                           int* count);
// 1.13c D2CMP+0x32EC
extern "C" void d2cmp_32ec(uint8_t b, int n, D2CommonStringStream2814* s, const D2CommonOutputCalls3985* calls,
                           int* count) {
    while (n > 0) {
        --n;
        d2cmp_32b9(b, s, calls, count);
        if (*count == -1) return;
    }
}
