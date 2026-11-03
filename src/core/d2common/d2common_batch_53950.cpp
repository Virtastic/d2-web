#include <cstdint>

// 1.13c D2Common+0x53950
// Copy up to 4 chars of src into dst, space-padding after the first NUL.
// Every case writes all four bytes: an empty src falls through to the shared fourth-byte store with
// the space it wrote to dst[0..2], so it becomes four spaces like any other padding.
extern "C" void D2Common_sub_53950(char* dst, const char* src) {
    bool pad = false;
    for (int i = 0; i < 4; i++) {
        if (!pad && src[i] == '\0') pad = true;
        dst[i] = pad ? ' ' : src[i];
    }
}
