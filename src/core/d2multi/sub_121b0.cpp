// 1.13c D2Multi+0x121B0: pointer one past the NUL of the string in EAX; null input is fatal in retail
// (error-box string 0xad via Ordinal_10265/10024, then _exit(-1)).
#include <cstdint>
#include <cstdlib>
extern "C" const char* d2_multi_sub_121b0(const char* s) {
    if (!s) std::_Exit(0xFF);   // ponytail: retail shows the error box first; UI not ported
    while (*s) ++s;
    return s + 1;
}
