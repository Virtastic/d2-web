#include "d2common_crt.h"

// 1.13c D2Common+0x079DB (127 B, caller 0x4C7E): __free_osfhnd. If fh is an open slot with a real OS
// handle, detach it (for fds 0-2 also clear the matching std handle when appType == 1) and set
// the slot handle to -1, returning 0; otherwise errno = EBADF (9), _doserrno = 0, return -1.
extern "C" int32_t d2common_free_osfhnd_79db(int32_t fh)
{
    if (static_cast<uint32_t>(fh) < g_d2common_crt.nhandle) {
        uint8_t* r = d2common_ioinfo_entry(fh);
        uint32_t h = r[0] | r[1] << 8 | r[2] << 16 | static_cast<uint32_t>(r[3]) << 24;
        if ((r[4] & 1) && h != 0xFFFFFFFFu) {
            if (g_d2common_crt.appType == 1) {
                if (fh == 0)      g_d2common_win32.SetStdHandle(0xFFFFFFF6u, 0);
                else if (fh == 1) g_d2common_win32.SetStdHandle(0xFFFFFFF5u, 0);
                else if (fh == 2) g_d2common_win32.SetStdHandle(0xFFFFFFF4u, 0);
            }
            r[0] = r[1] = r[2] = r[3] = 0xFF;
            return 0;
        }
    }
    *d2common_errno_44bc() = 9;
    g_d2common_crt.tdoserrno = 0;
    return -1;
}
