// Prior art used: none in D2MOO (host CRT); rules from the cached 1.13c body.
#include "d2common_crt.h"

// 1.13c D2Common+0x07255
// __lseek_lk. Bad handle -> errno EBADF, -1. On success clear osfile bit 0x02
// (FEOFLAG); on SetFilePointer failure (-1 with nonzero last error) map the DOS error and return -1.
// Win32 boundary: g_d2common_win32 SetFilePointer / GetLastError.
extern "C" uint32_t d2common_lseek_lk_7255(uint32_t fh, int32_t off, uint32_t origin)
{
    intptr_t h = d2common_get_osfhandle_7a5a(static_cast<int32_t>(fh));
    if (h == -1) { *d2common_errno_44bc() = 9; return 0xFFFFFFFFu; }
    uint32_t r = g_d2common_win32.SetFilePointer(h, off, origin);
    uint32_t err = (r == 0xFFFFFFFFu) ? g_d2common_win32.GetLastError() : 0;
    if (err == 0) { d2common_ioinfo_entry(static_cast<int32_t>(fh))[4] &= 0xFD; return r; }
    d2common_dosmaperr_44ce(err);
    return 0xFFFFFFFFu;
}
