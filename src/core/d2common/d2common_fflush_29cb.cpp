#include <cstdint>
#include <cstdio>

// Prior art: D2MOO does not provide the linked MSVC CRT body. The existing D2Common
// file wrappers (including D2Common_21F5_FClose) use host FILE objects; no retail
// _iobuf layout is overlaid on a host stream.
// 1.13c D2Common+0x029CB
// A null stream invokes flsall(0); otherwise retail locks the stream, calls its
// unlocked flush worker, and unlocks even during exception unwinding. Host fflush
// supplies that locking and the null/all-output-stream path. Preserve its 0/EOF
// result; this wrapper neither closes the stream nor forces a disk commit.
extern "C" int32_t D2Common_29CB_FFlush(FILE* stream)
{
    return static_cast<int32_t>(std::fflush(stream));
}
