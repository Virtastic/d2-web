// 1.13c D2Net: the CRT multibyte globals as one window, base RVA 0xB454, 0x22c bytes (to 0xB680). The ports of
// 0x3376, 0x339F and 0x36CE work on it; the FN probes pin the same range as one `gvar mb 0xb454 0x22c` blob.
#pragma once
#include <cstdint>
namespace d2net_mb {
struct Window {
    uint32_t lcid;            // +0x000 b454 __mblcid
    uint32_t mbinfo;          // +0x004 b458 __ptmbcinfo (the 0x220-byte snapshot a set-codepage call filled last)
    uint32_t ismbcodepage;    // +0x008 b45c
    uint8_t  mbctype[257];    // +0x00c b460 _mbctype (index 0 is EOF)
    uint8_t  pad10d[3];       // +0x10d
    uint32_t mbcodepage;      // +0x110 b564
    uint32_t gap114[2];       // +0x114 b568, b56c
    uint16_t mbulinfo[6];     // +0x11c b570 (setSBCS clears 3 dwords; the snapshot copies 5 words)
    uint32_t gap128;          // +0x128 b57c
    uint8_t  mbcasemap[256];  // +0x12c b580
};
static_assert(sizeof(Window) == 0x22c, "window size");
}
