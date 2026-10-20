#include <cstdint>
#include <cstdlib>

// Outlined cold-path of the +0x9900 read-file routine; takes its operands in
// ESI/EDI/stack-local rather than as arguments. Checks the archive handle
// (ESI), the Fog#10104 verdict (0 = read failed, Storm#276 is then called),
// and that the byte count read (stack local) equals the expected size (EDI).
// Any failed check formats an assert message (Fog#10265 string id 0x87, 0x8f
// or 0x92, Fog#10024 report) and _exit(-1). Success returns to the caller.
// Fog/Storm reporting is not reproduced; failure terminates the process.
// 1.13c D2Common+0x09610
void d2common_readfile_check_9610(const void* handle, int fogVerdict, uint32_t expected, uint32_t got)
{
    if (handle != nullptr && fogVerdict != 0 && expected == got)
        return;
    std::_Exit(-1); // ponytail: no Fog assert text; add when a vector needs the report calls
}
