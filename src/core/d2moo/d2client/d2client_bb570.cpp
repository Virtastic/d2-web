// 1.13c D2Client+0xBB570: EAX = n, EBX, EDI, ESI pass through to 0xB7B40 (EAX = ESI; stack EBX, EDI, count). 200 < n < 225 scatters
// twice with count 6; otherwise every fourth n scatters once with count 20. EAX at exit: the last scatter's, else the signed n % 4.
#include "d2c_seam.h"
extern "C" uint32_t d2c_bb570(const D2cSeam* s, int32_t n, uint32_t ebx, uint32_t edi, uint64_t esi) {
    if (n > 200 && n < 0xe1) {
        (*s)(0xb7b40, esi, 0, 0, 0, 0, ebx, edi, 6);
        return (uint32_t)(*s)(0xb7b40, esi, 0, 0, 0, 0, ebx, edi, 6);
    }
    int32_t m = n % 4;
    if (m) return (uint32_t)m;
    return (uint32_t)(*s)(0xb7b40, esi, 0, 0, 0, 0, ebx, edi, 0x14);
}
