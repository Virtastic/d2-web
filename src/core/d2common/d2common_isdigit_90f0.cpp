// _isdigit: ctype table lookup, _DIGIT bit (4).
// ponytail: C-locale table (only '0'..'9' carry the bit); retail reads the per-thread locale table.
#include <cstdint>

// 1.13c D2Common+0x090F0
extern "C"
int32_t D2Common_90F0_Isdigit(int32_t c)
{
    return (c >= '0' && c <= '9') ? 4 : 0;
}
