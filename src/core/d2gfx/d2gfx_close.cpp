// 1.13c D2Gfx+0x0755D (__close): VS2003 CRT descriptor close.
#include <cstdint>
#include <emscripten.h>

struct D2GfxCloseEnv {
    uint32_t nhandle;
    uint8_t* const* pioinfo;
    void (*lock_fhandle)(int32_t);       // 0x05CD4
    int32_t (*close_lk)(int32_t);        // 0x074DA
    void (*unlock_fhandle)();           // 0x075D4
    int32_t* (*errno_ptr)();            // 0x03858
    uint32_t* (*doserrno_ptr)();        // 0x03861
};

extern "C" int32_t d2gfx_close(
    int32_t file_handle, const D2GfxCloseEnv* env) {
    const uint32_t fd = static_cast<uint32_t>(file_handle);
    if (fd < env->nhandle) {
        // Each _pioinfo block has 32 records of 0x24 bytes; osfile is at +4.
        uint8_t* osfile = env->pioinfo[fd >> 5] + (fd & 31) * 0x24 + 4;
        if ((*osfile & 1) != 0) {
            env->lock_fhandle(file_handle);
            int32_t result;
            if ((*osfile & 1) == 0) {
                *env->errno_ptr() = 9;
                result = -1;
            } else {
                result = env->close_lk(file_handle);
            }
            env->unlock_fhandle();
            return result;
        }
    }
    *env->errno_ptr() = 9;
    *env->doserrno_ptr() = 0;
    return -1;
}
