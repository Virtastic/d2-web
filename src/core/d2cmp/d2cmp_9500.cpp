// 1.13c D2CMP+0x9500 (122 bytes): rectangle copy; ECX = destination pitch, EAX = source pitch.
#include <cstring>
#include "d2cmp_palstate.h"
// 1.13c D2CMP+0x09500
// Do-while over rows (rows==0 runs 2^32 times in retail, as here): width&3 head bytes, then width>>2
// dwords, then each pointer advances by its pitch minus width.
extern "C" void d2cmp_9500(uint8_t* dst, const uint8_t* src, int32_t rows, uint32_t width, int32_t dstPitch, int32_t srcPitch) {
    uint32_t n = (uint32_t)rows;
    do {
        uint32_t head = width & 3;
        for (uint32_t i = 0; i < head; ++i) dst[i] = src[i];
        dst += head; src += head;
        for (uint32_t i = width >> 2; i != 0; --i) { std::memcpy(dst, src, 4); dst += 4; src += 4; }
        src += srcPitch - (int32_t)width;
        dst += dstPitch - (int32_t)width;
    } while (--n != 0);
}
