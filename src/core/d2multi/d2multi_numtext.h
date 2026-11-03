// "%d" of v through retail's wsprintfA buffer and Unicode::toUnicode(dst, src, 2): count 2 keeps only the first character plus a NUL.
#pragma once
#include <cstdint>
static inline void d2m_numtext(uint16_t* out, int32_t v) {
    out[0] = (uint16_t)'-';
    if (v >= 0) {
        uint32_t u = (uint32_t)v;
        while (u >= 10) u /= 10;
        out[0] = (uint16_t)('0' + u);
    }
    out[1] = 0;
}
