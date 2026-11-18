// 1.13c BNClient+0x12F70 -- table init over the DAT_6ff3f62c table (five LCG dwords per row). Dependency of the 130B0 name hash.
#include <cstdint>
// base = the DAT_6ff3f62c table (0x1400 bytes): five LCG dwords per row, rows 0..255 at base+0x800 (+-0x800, +-0x400).
extern "C" uint32_t bnclient_12f70_table(uint32_t* base) {
    if (!base) return 0;
    uint32_t u1 = 0x100001, u2;
    uint32_t* p = base + 0x200;
    for (int i = 0; i < 0x100; ++i, ++p) {
        u1 = (u1 * 0x7d + 3) % 0x2aaaab; u2 = (u1 * 0x7d + 3) % 0x2aaaab;
        p[-0x200] = u1 << 0x10 | (u2 & 0xffff);
        u1 = (u2 * 0x7d + 3) % 0x2aaaab; u2 = (u1 * 0x7d + 3) % 0x2aaaab;
        p[-0x100] = u1 << 0x10 | (u2 & 0xffff);
        u1 = (u2 * 0x7d + 3) % 0x2aaaab; u2 = (u1 * 0x7d + 3) % 0x2aaaab;
        p[0] = u1 << 0x10 | (u2 & 0xffff);
        u1 = (u2 * 0x7d + 3) % 0x2aaaab; u2 = (u1 * 0x7d + 3) % 0x2aaaab;
        p[0x100] = u1 << 0x10 | (u2 & 0xffff);
        u2 = (u2 * 0x7d + 3) % 0x2aaaab; u1 = (u2 * 0x7d + 3) % 0x2aaaab;
        p[0x200] = u2 << 0x10 | (u1 & 0xffff);
    }
    return u1;   // retail leaves the last walk value in EAX (replay: row 0x12F70 eax)
}
