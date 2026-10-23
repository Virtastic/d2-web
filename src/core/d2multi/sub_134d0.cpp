// 1.13c D2Multi+0x134D0: character-class gate. EAX = ptr to a byte (an ASCII-digit-based code);
// reads the 16-bit word at DAT_6fa09f14+0x1eb (passed here as `flags`). Returns nonzero if allowed.
#include <cstdint>
extern "C" int32_t d2_multi_sub_134d0(const uint8_t* p, uint16_t flags) {
    uint8_t b = *p;
    int32_t lim = ((flags >> 8) & 0x1f) / 5 + 1;
    if (!(flags & 4)) return lim < (int32_t)b - 0x30;
    if ((int32_t)b - 0x33 <= lim && b > 0x32) return 0;
    return 1;
}
