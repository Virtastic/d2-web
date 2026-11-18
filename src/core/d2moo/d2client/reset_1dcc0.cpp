// 1.13c D2Client+0x1DCC0
#include <cstdint>
#include <cstring>
// z = bytes at RVA 0x123365 (0x34 long), k = bytes at RVA 0x1223F5 (8 long). Returns 1 if the callee chain completed (nothing written).
extern "C" uint32_t d2_client_1dcc0(uint32_t h, uint32_t a, uint32_t b, uint8_t* z, uint8_t* k) {
    if (h && a && b) return 1;
    // all 13 stores are 32-bit (89 35 = mov [m],esi), 0x365..0x395 step 4: z is zeroed whole
    memset(z, 0, 0x34);
    memset(k, 0xFF, 8);
    return 0;
}
