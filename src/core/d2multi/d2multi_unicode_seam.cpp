// Wide-string helpers the multiplayer list builder calls through C names (copy and concatenate on 16-bit characters).
#include <cstdint>
extern "C" {
uint16_t* ustrncat(uint16_t* dst, const uint16_t* src, int32_t count) {
    int32_t d = 0, i = 0;
    while (dst[d]) ++d;
    for (; i != count && src[i]; ++i) dst[d + i] = src[i];
    dst[d + i] = 0;
    return dst;
}
void uwin2Unicode(uint16_t* dst, const char* src, int32_t count) {
    int32_t i = 0;
    for (; i < count; ++i) {
        uint8_t c = (uint8_t)src[i];
        if (!c) break;
        dst[i] = c;
    }
    if (src[i]) { dst[i - 1] = 0; return; }
    dst[i] = 0;
}
}
