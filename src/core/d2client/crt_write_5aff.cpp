// src/core/d2client/crt_write_5aff.cpp -- 1.13c D2Client.dll alpha-lane row, from decomp/0x5AFF.c.
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_pioinfo[0x20];   // +0x1239A0 DAT_6fbd39a0 fd table (d2client_batch_03f82.cpp)
extern uint32_t g_fdcount;         // +0x123990 _nfile (d2client_batch_b56.cpp)
extern "C" int32_t* d2_client_579c(void);   // &errno
extern "C" int32_t* d2_client_57a5(void);  // &_doserrno

struct D2ClientWriteEnv {
    int32_t (*lock_fhandle)(int32_t fd);                              // 0xA67B
    int32_t (*write_lk)(int32_t fd, const void* buf, uint32_t n);     // 0x5943
    void    (*unlock_fhandle)(uint32_t fd);                           // 0x5B86 -> 0xA71B
};

static int32_t bad_fd() {
    *d2_client_579c() = 9;   // EBADF
    *d2_client_57a5() = 0;
    return -1;
}

// 1.13c D2Client+0x05AFF (160 B, callers 0x3704, 0x4AE8, 0x5BAA): __write(fd, buf, count).
// Reject fd >= _nfile or a descriptor whose ioinfo flag (+4) bit 0 (FOPEN) is clear with errno 9.
// Otherwise lock the fd, re-test FOPEN under the lock (a close may have raced), call _write_lk, unlock.
extern "C"
EMSCRIPTEN_KEEPALIVE int32_t d2_client_5aff(int32_t fd, const void* buf, uint32_t count,
                                            const D2ClientWriteEnv* env) {
    if ((uint32_t)fd >= g_fdcount) return bad_fd();
    const uint8_t* flag = (const uint8_t*)(uintptr_t)(g_pioinfo[(uint32_t)fd >> 5] +
                                                      ((uint32_t)fd & 0x1F) * 0x24 + 4);
    if (!(*flag & 1)) return bad_fd();
    env->lock_fhandle(fd);
    int32_t r;
    if (!(*flag & 1)) r = bad_fd();
    else r = env->write_lk(fd, buf, count);
    env->unlock_fhandle((uint32_t)fd);   // retail 0x5B86 with fd live in EBX
    return r;
}
