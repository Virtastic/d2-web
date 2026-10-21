#include <cstdint>

// CRT globals/imports of __free_osfhnd. An ioinfo entry is 0x24 bytes: +0 osfhnd, +4 osfile.
struct D2FreeOsfhndEnv {
    uint32_t  nhandle;                          // DAT 0x6FDF1714
    uint32_t* (*ioinfo_entry)(int32_t fh);      // &DAT_6fdf1720[fh>>5][fh&31]
    int32_t   stdhandle_mode;                   // DAT 0x6FDEF5F8 (1 = console app handles)
    int32_t   (*set_std_handle)(uint32_t n, uint32_t h); // SetStdHandle
    int32_t*  (*errno_ptr)();                   // +0x44BC
    uint32_t* (*doserrno_ptr)();                // +0x44C5
};

// 1.13c D2Common+0x079DB (127 B, caller 0x4C7E): __free_osfhnd. If fh is an open slot with a real OS
// handle, detach it (for fds 0-2 also clear the matching std handle when stdhandle_mode == 1) and set
// the slot to -1, returning 0; otherwise errno = EBADF (9), _doserrno = 0, return -1.
extern "C" int32_t d2common_free_osfhnd(int32_t fh, const D2FreeOsfhndEnv* env)
{
    if ((uint32_t)fh < env->nhandle) {
        uint32_t* e = env->ioinfo_entry(fh);
        if ((e[1] & 1) && e[0] != 0xFFFFFFFFu) {
            if (env->stdhandle_mode == 1) {
                if (fh == 0)      env->set_std_handle(0xFFFFFFF6u, 0);
                else if (fh == 1) env->set_std_handle(0xFFFFFFF5u, 0);
                else if (fh == 2) env->set_std_handle(0xFFFFFFF4u, 0);
            }
            e[0] = 0xFFFFFFFFu;
            return 0;
        }
    }
    *env->errno_ptr() = 9;
    *env->doserrno_ptr() = 0;
    return -1;
}
