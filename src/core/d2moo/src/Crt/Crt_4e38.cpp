// 1.13c D2Common+0x4E38 __commit(fd): the statically linked MSVC CRT flush-to-disk leaf. D2MOO has no twin (it links the host CRT).
// Retail: validate fd against the handle table (bad fd = EBADF, -1), lock the fhandle, and if the handle is open call
// FlushFileBuffers (failure = doserrno, -1), unlock. Host fd stands for the handle; fsync on it, 0 on success, -1 otherwise.
// Only caller is __fflush_lk (stdio flush).
#include <cstdint>
#include <unistd.h>

// 1.13c D2Common+0x4E38
extern "C" int32_t D2Common_4E38_Commit(int32_t fd)
{
    return fsync(fd) != 0 ? -1 : 0;
}
