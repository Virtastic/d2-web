// Prior art: no D2MOO D2Client module exists. The adjacent D2Client CRT rows
// (crt_read_8383.cpp and crt_lseeki64_lk_a4bc.cpp) establish the shared descriptor table
// and explicit Win32 host seams; the cached 1.13c body controls this function's branches.
#include <cstddef>
#include <cstdint>
#include <emscripten.h>

extern uint32_t g_fdcount;      // 0x123990, _nfile
extern uint32_t g_pioinfo[0x20]; // 0x1239A0, descriptor block pointers
extern "C" int32_t* d2_client_579c(void); // 0x0579C, _errno
extern "C" int32_t* d2_client_57a5(void); // 0x057A5, _doserrno

struct D2ClientCommitIoInfo {
    uint32_t os_handle;
    uint8_t flags;
    uint8_t reserved[0x1f];
};
static_assert(sizeof(D2ClientCommitIoInfo) == 0x24);
static_assert(offsetof(D2ClientCommitIoInfo, flags) == 4);

struct D2ClientCommitEnv {
    void (*lock_fhandle)(uint32_t fd);          // 0x0A67B
    uint32_t (*get_osfhandle)(uint32_t fd);      // 0x0A63A
    int32_t (*flush_file_buffers)(uint32_t handle); // kernel32 FlushFileBuffers
    uint32_t (*get_last_error)(void);            // kernel32 GetLastError
    void (*unlock_fhandle)(void);                // finally thunk 0x0A959
};

static uint8_t* d2_client_commit_record(uint32_t fd) {
    auto* block = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(g_pioinfo[fd >> 5]));
    return block + (fd & 31u) * 0x24u;
}

// 1.13c D2Client+0x0A8B9: _commit(fd) validates and locks an in-use descriptor,
// flushes its OS handle, stores a nonzero Win32 error in _doserrno, and returns
// -1 with errno 9 on failure. A zero GetLastError after a failed flush is success.
extern "C" EMSCRIPTEN_KEEPALIVE int32_t d2_client_a8b9(uint32_t fd,
                                                       const D2ClientCommitEnv* env) {
    if (fd >= g_fdcount || (d2_client_commit_record(fd)[4] & 1u) == 0) {
        *d2_client_579c() = 9;
        return -1;
    }

    env->lock_fhandle(fd);
    int32_t result = -1;
    if ((d2_client_commit_record(fd)[4] & 1u) != 0) {
        const uint32_t handle = env->get_osfhandle(fd);
        const uint32_t error = env->flush_file_buffers(handle) == 0
                                   ? env->get_last_error()
                                   : 0;
        if (error == 0) {
            result = 0;
        } else {
            *d2_client_57a5() = static_cast<int32_t>(error);
        }
    }
    if (result != 0) *d2_client_579c() = 9;
    env->unlock_fhandle();
    return result;
}
