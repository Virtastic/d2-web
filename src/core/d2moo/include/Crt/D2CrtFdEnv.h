#pragma once
// Host seams of the CRT descriptor functions (__write 0x481F, __lseek 0x72C9): the ioinfo table, errno slots and lock calls.
#include <cstdint>

struct D2CrtFdEnv {
    uint32_t nhandle;                              // DAT_6fdf1714
    uint8_t* (*ioinfo_entry)(int32_t handle);      // record (0x24 B) of the handle's 32-slot block, or null
    void (*lock_fhandle)(int32_t handle);          // 0x7A9B
    void (*unlock_fhandle)(int32_t handle);        // 0x48A6 / 0x7350
    void (*set_errno)(int32_t value);              // 0x44BC slot
    void (*set_doserrno)(uint32_t value);          // 0x44C5 slot
    int32_t (*write_nolock)(int32_t, const void*, uint32_t);   // 0x4663
    int32_t (*lseek_nolock)(int32_t, int32_t, int32_t);        // __lseek_lk 0x7255
};
