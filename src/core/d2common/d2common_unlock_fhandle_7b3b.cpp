#include <cstdint>

// CRT __unlock_fhandle: leave the critical section at ioinfo record + 0xC for the handle.
// The ioinfo table (8-byte-entry blocks of 32 records x 0x24 B) and LeaveCriticalSection are
// supplied by the caller.
struct D2UnlockFHandleEnv {
    uint8_t* (*ioinfo_entry)(int32_t handle);
    void (*leave_critical_section)(void* cs);
};

// 1.13c D2Common+0x07B3B
extern "C" void d2common_unlock_fhandle(int32_t handle, const D2UnlockFHandleEnv* env)
{
    env->leave_critical_section(env->ioinfo_entry(handle) + 0xC);
}
