// 1.13c D2Multi+0x134D0: EAX = pointer to a digit byte; flags = the word at (DAT_6fa09f14)+0x1eb. Level limit = ((flags >> 8) & 0x1f) / 5 + 1.
// Bit 2 clear: the byte is allowed when limit < digit. Bit 2 set: allowed unless the digit is above '2' and at most limit + 3.
#include <cstdint>

uint32_t d2multi_134d0(const uint8_t* p, uint32_t flags) {
    uint32_t b = *p;
    int32_t lim = (int32_t)((flags >> 8) & 0x1f) / 5 + 1;
    if (!(flags & 4)) return lim < (int32_t)b - 0x30;
    if ((int32_t)b - 0x33 <= lim && b > 0x32) return 0;
    return 1;
}
