#include <cstdint>
#include <cstdlib>

// Outlined cold-path of the +0x9900 read-file routine (ESI = archive handle, one stack arg,
// ret 4). A null handle goes straight to the fatal path (string id 0x59). Otherwise the
// Storm call at +0x939A (ECX = handle, EDX = the stack arg) runs; a nonzero result returns.
// A zero result calls Storm +0x9364 (handle, 260-byte stack buffer) and takes the fatal
// path (string id 0x5F). Fatal = Storm string lookup, Fog report, _exit(-1).
// Storm/Fog reporting is not reproduced; failure terminates the process.
// 1.13c D2Common+0x09700
void d2common_readfile_close_9700(const void* handle, uint32_t arg, int stormResult)
{
    (void)arg;
    if (handle != nullptr && stormResult != 0)
        return;
    std::_Exit(-1); // ponytail: no report text; add when a vector needs the report calls
}
