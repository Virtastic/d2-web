// Prior art used: none in D2MOO (host CRT); rule from the cached 1.13c body, env-seam shape as
// d2common_crt_write_lseek.cpp (ioinfo table is a host seam).
#include <cstdint>

// 1.13c D2Common+0x073B8: __isatty. Handle >= nhandle (DAT_6fdf1714) -> 0; else osfile byte
// (record+4 of the 0x24-byte ioinfo entry) & 0x40 (FDEV), returned as 0 or 0x40.
extern "C" int32_t d2common_isatty_73b8(int32_t fh, uint32_t nhandle, uint8_t* (*ioinfo_entry)(int32_t))
{
    if (static_cast<uint32_t>(fh) >= nhandle) return 0;
    return ioinfo_entry(fh)[4] & 0x40;
}
