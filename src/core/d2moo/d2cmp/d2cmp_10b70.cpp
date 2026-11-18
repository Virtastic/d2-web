// 1.13c D2CMP+0x10B70 (export-free, 86 bytes): __regs(eax) -> int. Scans 15 rows of a 0x20-byte table
// at the argument for a zero byte. Row i reads iVar2 bytes (4, 8, ... 32, then 28 ... down) starting at
// row base + iVar3 (14, 12, ... then up); the first zero byte found returns 1, otherwise 0.
#include <cstdint>

extern "C" uint32_t d2cmp_10b70(const uint8_t* base) {
    int iVar4 = 0;
    int iVar3 = 0xe;
    int iVar2 = 4;
    do {
        int iVar1 = 0;
        if (0 < iVar2) {
            do {
                if (base[iVar3 + iVar1] == 0) return 1;
                iVar1 = iVar1 + 1;
            } while (iVar1 < iVar2);
        }
        base += 0x20;
        if (iVar4 < 7) { iVar3 -= 2; iVar2 += 4; }
        else           { iVar3 += 2; iVar2 -= 4; }
        iVar4 = iVar4 + 1;
    } while (iVar4 < 0xf);
    return 0;
}
