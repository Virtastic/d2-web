// 1.13c D2Client+0x1DCC0
#include <cstdint>
#include <cstring>
// z = bytes at RVA 0x123365 (0x34 long), k = bytes at RVA 0x1223F5 (8 long). Returns 1 if the callee chain completed (nothing written).
extern "C" uint32_t d2_client_1dcc0(uint32_t h, uint32_t a, uint32_t b, uint8_t* z, uint8_t* k) {
    if (h && a && b) return 1;
    z[0x00] = 0; z[0x30] = 0; z[0x18] = 0;   // +0x365 +0x395 +0x37d
    static const uint8_t d[] = { 0x04, 0x08, 0x0c, 0x10, 0x14, 0x1c, 0x20, 0x24, 0x28, 0x2c };  // dwords 369 36d 371 375 379 381 385 389 38d 391
    for (uint8_t o : d) memset(z + o, 0, 4);
    memset(k, 0xFF, 8);
    return 0;
}
