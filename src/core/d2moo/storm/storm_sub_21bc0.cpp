// 1.13c Storm+0x21BC0 -- as 0x21B30 but mono (1 channel).
#include <cstdint>

extern "C" uint32_t Storm_Sub21780(int16_t* out, uint32_t outSize, const int16_t* in, uint32_t ch, uint8_t shift, uint32_t inBytes);

// 1.13c Storm+0x21BC0
extern "C" void Storm_Sub21BC0(int16_t* out, uint32_t* pOutSize, const int16_t* in, uint32_t inBytes,
                               uint32_t* pOutShift, uint32_t level) {
    uint8_t shift = 5;
    *pOutShift = 7;
    if (level != 0) {
        if (level < 3) { shift = 4; *pOutShift = 6; }
        else if (level == 3) { shift = 6; *pOutShift = 8; }
    }
    *pOutSize = Storm_Sub21780(out, *pOutSize, in, 1, shift, inBytes);
}
