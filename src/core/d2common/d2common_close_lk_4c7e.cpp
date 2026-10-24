#include "d2common_crt.h"

// 1.13c D2Common+0x04C7E (131 B, callers 0x75A0, 0x4D01): __close_lk. Close the OS handle of a CRT fd
// (skipped when fd is 1 or 2 and stdout/stderr share one OS handle, or the fd has no OS handle), release
// the slot, clear its osfile flags, map a CloseHandle error to errno. Returns 0 or -1.
extern "C" int32_t d2common_close_lk_4c7e(uint32_t fh)
{
    uint32_t err = 0;
    if (d2common_get_osfhandle_7a5a(static_cast<int32_t>(fh)) != -1) {
        bool skip = (fh == 1 || fh == 2) && d2common_get_osfhandle_7a5a(2) == d2common_get_osfhandle_7a5a(1);
        if (!skip && !g_d2common_win32.CloseHandle(d2common_get_osfhandle_7a5a(static_cast<int32_t>(fh))))
            err = g_d2common_win32.GetLastError();
    }
    d2common_free_osfhnd_79db(static_cast<int32_t>(fh));
    d2common_ioinfo_entry(static_cast<int32_t>(fh))[4] = 0;
    if (err == 0) return 0;
    d2common_dosmaperr_44ce(err);
    return -1;
}
