#include <cstdint>
#include <cstdio>

// 1.13c D2Common+0x432A
// Retail __ftbuf(flag, stream): when flag is nonzero and the stream carries the temporary-buffer bit
// (_flag 0x1000), flush it (__flush), clear that bit plus the 0x10 bit of the flag high byte, and zero
// bufsiz/ptr/base. Host stdio owns buffering, so the port only flushes when the retail guard would fire
// is unobservable; the flag is honoured and the stream is flushed.
extern "C" void D2Common_432A_Ftbuf(int32_t flag, FILE* stream)
{
    if (flag != 0 && stream != nullptr)
        std::fflush(stream);
}
