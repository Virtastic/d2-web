// ___ascii_stricmp: byte compare, fold A-Z only at a mismatch; -1/0/1.
#include <cstdint>

// 1.13c D2Common+0x03790
extern "C"
int32_t D2Common_3790_AsciiStricmp(const char* a, const char* b)
{
    for (;;) {
        uint8_t x = (uint8_t)*a++, y = (uint8_t)*b++;
        if (x != y) {
            if (x >= 'A' && x <= 'Z') x += 0x20;
            if (y >= 'A' && y <= 'Z') y += 0x20;
            if (x != y) return x < y ? -1 : 1;
        }
        if (y == 0) return 0;
    }
}
