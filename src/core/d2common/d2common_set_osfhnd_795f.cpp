#include <cstdint>

// CRT globals/imports of __set_osfhnd. An ioinfo entry is 0x24 bytes: +0 osfhnd.
struct D2SetOsfhndEnv {
    uint32_t  nhandle;                          // DAT 0x6FDF1714
    uint32_t* (*ioinfo_entry)(int32_t fh);      // &DAT_6fdf1720[fh>>5][fh&31]
    int32_t   stdhandle_mode;                   // DAT 0x6FDEF5F8 (1 = console app handles)
    int32_t   (*set_std_handle)(uint32_t n, uint32_t h); // SetStdHandle
    int32_t*  (*errno_ptr)();                   // +0x44BC
    uint32_t* (*doserrno_ptr)();                // +0x44C5
};

// 1.13c D2Common+0x0795F (124 B, caller 0x75A0): __set_osfhnd. If fh is in range and its slot is
// free (-1), store the OS handle (for fds 0-2 also SetStdHandle when stdhandle_mode == 1), return 0;
// otherwise errno = EBADF (9), _doserrno = 0, return -1.
extern "C" int32_t d2common_set_osfhnd(int32_t fh, uint32_t osh, const D2SetOsfhndEnv* env)
{
    if ((uint32_t)fh < env->nhandle) {
        uint32_t* e = env->ioinfo_entry(fh);
        if (e[0] == 0xFFFFFFFFu) {
            if (env->stdhandle_mode == 1) {
                if (fh == 0)      env->set_std_handle(0xFFFFFFF6u, osh);
                else if (fh == 1) env->set_std_handle(0xFFFFFFF5u, osh);
                else if (fh == 2) env->set_std_handle(0xFFFFFFF4u, osh);
            }
            e[0] = osh;
            return 0;
        }
    }
    *env->errno_ptr() = 9;
    *env->doserrno_ptr() = 0;
    return -1;
}
