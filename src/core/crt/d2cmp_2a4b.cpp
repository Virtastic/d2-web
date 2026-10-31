// 1.13c D2CMP+0x2A4B (_vsprintf, VS2003 CRT): instruction-identical to D2Common+0x2814 once
// addresses are normalised. A string stream {dest, INT32_MAX, dest, 0x42} is formatted by _output
// (+0x3347 = D2Common+0x3985), then the count drops by one and a NUL is stored (flush when exhausted).
// The variadic block is the D2Common argument source (wasm64 has no 32-bit stack argument block).
#include <bit>
#include "core/d2common/output_3985.h"
// 1.13c D2CMP+0x2A4B
extern "C" int d2cmp_2a4b(char* dest, const char* fmt, const D2CommonOutputArgs3985* args,
                         const D2CommonOutputCalls3985* calls) {
    D2CommonStringStream2814 s{dest, INT32_MAX, dest, 0x42};
    const int r = D2Common_3985_Output(&s, fmt, args, calls);
    s.remaining = std::bit_cast<int32_t>(uint32_t(s.remaining) - 1u);
    if (s.remaining < 0) calls->flushByte(0, &s);
    else *s.cursor = '\0';
    return r;
}
