#include <cstdint>
#include <cstdio>

// 1.13c D2Common+0x215D
// The retail wrapper acquires the FILE lock, delegates the element write to __fwrite_lk, unlocks
// through its SEH cleanup, and returns the number of complete elements written. Host stdio performs
// that operation and lock lifetime in one call.
extern "C" uint32_t D2Common_215D_Fwrite(const void* data, uint32_t size, uint32_t count, FILE* stream)
{
    return static_cast<uint32_t>(std::fwrite(data, static_cast<size_t>(size),
                                             static_cast<size_t>(count), stream));
}
