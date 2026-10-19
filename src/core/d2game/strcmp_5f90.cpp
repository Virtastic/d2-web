// 1.13c D2Game+0x05F90 (_strcmp): compare NUL-terminated strings as unsigned bytes.
#include <cstdint>

extern "C" int32_t d2game_strcmp_5f90(const uint8_t* a, const uint8_t* b) {
    for (;; ++a, ++b) {
        if (*a != *b) return *a < *b ? -1 : 1;
        if (*a == 0) return 0;
    }
}
