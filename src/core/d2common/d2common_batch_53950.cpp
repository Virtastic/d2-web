#include <cstdint>

// 1.13c D2Common+0x53950
// Copy up to 4 chars of src into dst, space-padding after the first NUL.
// Retail writes only dst[0..2] when src is empty (its fourth byte is left untouched); every other case
// writes all four bytes, padding with spaces after the first NUL.
extern "C" void D2Common_sub_53950(char* dst, const char* src) {
    if (src[0] == '\0') {
        dst[0] = dst[1] = dst[2] = ' ';
        return;
    }
    bool pad = false;
    for (int i = 0; i < 4; i++) {
        if (!pad && src[i] == '\0') pad = true;
        dst[i] = pad ? ' ' : src[i];
    }
}
