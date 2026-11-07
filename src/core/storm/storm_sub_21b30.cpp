// 1.13c Storm+0x21B30 -- picks (shift, *outShift) from the level and encodes stereo (2 channels) with 0x21780.
// level 0 or >=4: shift 5, *outShift 7; 1,2: 4, 6; 3: 6, 8. The encoder's returned position replaces *outSize.
// Retail passes its own stack argument inBytes on in EAX to 0x21780.
#include <cstdint>

extern "C" uint32_t Storm_Sub21780(int16_t* out, uint32_t outSize, const int16_t* in, uint32_t ch, uint8_t shift, uint32_t inBytes);

// 1.13c Storm+0x21B30
extern "C" void Storm_Sub21B30(int16_t* out, uint32_t* pOutSize, const int16_t* in, uint32_t inBytes,
                               uint32_t* pOutShift, uint32_t level) {
    uint8_t shift = 5;
    *pOutShift = 7;
    if (level != 0) {
        if (level < 3) { shift = 4; *pOutShift = 6; }
        else if (level == 3) { shift = 6; *pOutShift = 8; }
    }
    *pOutSize = Storm_Sub21780(out, *pOutSize, in, 2, shift, inBytes);
}
