#include <cstdint>
extern int32_t* DAT_6f9c6580; extern int32_t DAT_6f9c658c, DAT_6f9c5994; // host seams
void D2sound_7650(void*); void D2sound_6E20(void*, int); int D2sound_66F0(void*, int, int); // ported/host callees
// 1.13c D2sound+0x7C60 start a sound on the current channel; 1 on success.
uint32_t D2sound_7C60(int32_t a, int32_t b) {
    if (DAT_6f9c6580 && DAT_6f9c658c && DAT_6f9c5994 && DAT_6f9c6580[0x12] == 0 && a) {
        D2sound_7650(DAT_6f9c6580);
        D2sound_6E20(DAT_6f9c6580, 0x6e);
        if (D2sound_66F0(DAT_6f9c6580, 0, b)) return 1;
    }
    return 0;
}
