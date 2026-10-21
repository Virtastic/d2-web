#include <cstdint>

// CRT callbacks of __close_lk; the ioinfo table (DAT 0x6FDF1720) is reached through ioinfo_osfile.
struct D2CloseLkEnv {
    intptr_t (*get_osfhandle)(int32_t fh);        // +0x7A5A
    int32_t  (*close_handle)(intptr_t h);         // CloseHandle
    uint32_t (*get_last_error)();                 // GetLastError
    void     (*free_osfhnd)(int32_t fh);          // +0x79DB
    uint8_t* (*ioinfo_osfile)(int32_t fh);        // &ioinfo[fh>>5][fh&31].osfile (entry+4)
    void     (*dosmaperr)(uint32_t err);          // +0x44CE
};

// 1.13c D2Common+0x04C7E (131 B, callers 0x75A0, 0x4D01): __close_lk. Close the OS handle of a CRT fd
// (skipped when fd is 1 or 2 and stdout/stderr share one OS handle, or the fd has no OS handle), release
// the slot, clear its osfile flags, map a CloseHandle error to errno. Returns 0 or -1.
extern "C" int32_t d2common_close_lk(uint32_t fh, const D2CloseLkEnv* env)
{
    uint32_t err = 0;
    if (env->get_osfhandle((int32_t)fh) != -1) {
        bool skip = false;
        if (fh == 1 || fh == 2)
            skip = env->get_osfhandle(1) == env->get_osfhandle(2);
        if (!skip && !env->close_handle(env->get_osfhandle((int32_t)fh)))
            err = env->get_last_error();
    }
    env->free_osfhnd((int32_t)fh);
    *env->ioinfo_osfile((int32_t)fh) = 0;
    if (err == 0) return 0;
    env->dosmaperr(err);
    return -1;
}
