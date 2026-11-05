// 1.13c D2Client+0x8C720: ESI = wide string. Finds the last ']' (scanning down from the end, index
// 0 included); when it is at index n >= 3 and s[n-3] is '[', returns s + n + 1 (the text after a "[xy]" tag), otherwise s itself.
#include "cl_env.h"
extern "C" uint32_t d2cl_8c720(uint32_t s, const cl::Env& e) {
    auto at = [&](int32_t i) { return e.R(s + 2u * (uint32_t)i) & 0xffff; };
    int32_t n = 0;
    while (at(n)) ++n;                       // Unicode strlen (import 0xCEAB0)
    while (n > 0 && at(n) != 0x5d) --n;
    if (at(n) != 0x5d || n < 3) return s;
    return at(n - 3) == 0x5b ? s + 2u * (uint32_t)n + 2u : s;
}
