#include <cstdint>

// 1.13c D2Gfx+0x0386A __dosmaperr (115 bytes): store the DOS error in ptd->_tdoserrno (+0xC), then set
// ptd->_terrno (+8) from the 0x2D-entry (dos, errno) dword-pair table at D2Gfx+0x902C0 (module data, passed
// in as `table`); no hit: 0x13..0x24 -> 13 (EACCES), 0xBC..0xCA -> 8 (ENOEXEC), else 22 (EINVAL).
// ptd is what __getptd (0x28FE) returns.
extern "C" void d2gfx_dosmaperr_ptd(uint8_t* ptd, uint32_t dos_error, const uint32_t* table) {
    *(uint32_t*)(ptd + 0xC) = dos_error;
    int32_t* terrno = (int32_t*)(ptd + 8);
    for (uint32_t i = 0; i < 0x2D; ++i)
        if (dos_error == table[i * 2]) { *terrno = (int32_t)table[i * 2 + 1]; return; }
    if (dos_error > 0x12 && dos_error < 0x25) *terrno = 0xD;
    else if (dos_error > 0xBB && dos_error < 0xCB) *terrno = 8;
    else *terrno = 0x16;
}
