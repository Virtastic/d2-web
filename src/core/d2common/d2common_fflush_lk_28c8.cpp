#include <cstdint>
#include <cstdio>

// Prior art: none in D2MOO (linked MSVC CRT); sibling D2Common_29CB_FFlush uses host stdio.
// 1.13c D2Common+0x028C8 (__fflush_lk)
// Retail: __flush(stream) failing -> -1; then a stream flagged 0x4000 also commits its fd
// (__commit, nonzero -> -1); else 0. Host stdio owns the buffer and fd, so host fflush
// stands for flush+commit; 0/-1 result, no retail _iobuf overlay.
extern "C" int32_t D2Common_28C8_FFlushLk(FILE* stream)
{
    return std::fflush(stream) != 0 ? -1 : 0;
}
